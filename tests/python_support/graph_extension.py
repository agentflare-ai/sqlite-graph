import json
import os
import subprocess
import sys
from pathlib import Path
from typing import List, Optional, Sequence, Union


class GraphCliRunner:
  """Execute SQL statements through the bundled sqlite3 CLI."""

  def __init__(self, repo_root: Union[str, Path], database: str = ":memory:") -> None:
    self.repo_root = Path(repo_root)
    self.database = database
    self.sqlite_bin = self.repo_root / "build" / "sqlite3"
    if not self.sqlite_bin.exists():
      raise FileNotFoundError(
          f"sqlite3 binary not found at {self.sqlite_bin}. Run 'make' before invoking CLI tests.")
    self.extension_path = self.repo_root / "build" / self._lib_name()
    if not self.extension_path.exists():
      raise FileNotFoundError(
          f"Graph extension not found at {self.extension_path}. Build the project before running tests.")

  def _lib_name(self) -> str:
    return "libgraph.dylib" if sys.platform == "darwin" else "libgraph.so"

  def run(self, statements: Union[str, Sequence[str]]) -> List[dict]:
    if isinstance(statements, str):
      script_body = statements
    else:
      script_body = "\n".join(statements)

    script = (f".bail on\n"
              f".load \"{self.extension_path}\"\n"
              ".mode json\n"
              ".headers on\n"
              f"{script_body}\n")

    env = os.environ.copy()
    lib_var = "DYLD_LIBRARY_PATH" if sys.platform == "darwin" else "LD_LIBRARY_PATH"
    lib_dir = str(self.extension_path.parent)
    existing = env.get(lib_var)
    env[lib_var] = lib_dir if not existing else f"{lib_dir}:{existing}"

    proc = subprocess.run(
        [str(self.sqlite_bin), self.database],
        input=script,
        text=True,
        capture_output=True,
        env=env,
        check=False,
    )
    if proc.returncode != 0:
      raise RuntimeError(f"sqlite3 CLI failed: {proc.stderr.strip()}")

    rows: List[dict] = []
    for line in proc.stdout.splitlines():
      line = line.strip()
      if not line:
        continue
      try:
        rows.append(json.loads(line))
      except json.JSONDecodeError:
        rows.append({"raw": line})
    return rows


class GraphTestHarness:
  """Utility that provides a connection-like API for Python tests."""

  def __init__(self, database: str = ":memory:", prefer_cli: bool = False) -> None:
    self.database = database
    self.repo_root = Path(__file__).resolve().parents[1]
    self.backend: str
    self.conn = None
    self.cli: Optional[GraphCliRunner] = None
    self._extension_path = self.repo_root / "build" / self._lib_name()
    if prefer_cli:
      self._use_cli_backend()
    else:
      if not self._try_sqlite_backend():
        self._use_cli_backend()

  def _lib_name(self) -> str:
    return "libgraph.dylib" if sys.platform == "darwin" else "libgraph.so"

  def _try_sqlite_backend(self) -> bool:
    conn = None
    try:
      try:
        import pysqlite3 as sqlite3_module  # type: ignore
      except ImportError:
        import sqlite3 as sqlite3_module  # type: ignore
      conn = sqlite3_module.connect(self.database)
      if not hasattr(conn, "enable_load_extension"):
        conn.close()
        return False
      conn.enable_load_extension(True)
      conn.load_extension(str(self._extension_path))
      conn.execute("CREATE VIRTUAL TABLE IF NOT EXISTS graph USING graph()")
      self.conn = conn
      self.backend = "sqlite"
      return True
    except Exception:
      if conn is not None:
        try:
          conn.close()
        except Exception:
          pass
      self.conn = None
      return False

  def _use_cli_backend(self) -> None:
    self.cli = GraphCliRunner(self.repo_root, self.database)
    self.backend = "cli"

  def execute(self, sql: str, params: Optional[Sequence] = None):
    if self.backend != "sqlite":
      raise RuntimeError("CLI backend active; use run_script() for executing statements.")
    return self.conn.execute(sql, params or ())

  def run_script(self, statements: Union[str, Sequence[str]]) -> List[dict]:
    if self.backend == "sqlite":
      if isinstance(statements, str):
        self.conn.executescript(statements)
      else:
        self.conn.executescript("\n".join(statements))
      return []
    assert self.cli is not None
    return self.cli.run(statements)

  def close(self) -> None:
    if self.conn is not None:
      self.conn.close()
      self.conn = None

  def __enter__(self):
    return self

  def __exit__(self, exc_type, exc, tb):
    self.close()


def connect_graph(database: str = ":memory:", prefer_cli: bool = False) -> GraphTestHarness:
  """Factory for Python tests. Falls back to CLI if extension loading is unavailable."""
  return GraphTestHarness(database=database, prefer_cli=prefer_cli)
