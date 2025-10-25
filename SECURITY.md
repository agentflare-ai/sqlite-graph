# Security Policy

## Supported Versions

We take security vulnerabilities seriously. The following versions are currently supported with security updates:

| Version | Supported          | Status |
| ------- | ------------------ | ------ |
| 0.1.x   | :white_check_mark: | Alpha  |

Note: As this is an alpha release, security updates will be provided on a best-effort basis.

## Reporting a Vulnerability

**Please do not report security vulnerabilities through public GitHub issues.**

If you discover a security vulnerability in the SQLite Graph Database Extension, please report it to us privately:

### Email

Send details to: **security@agentflare.ai**

### What to Include

When reporting a vulnerability, please include:

1. **Description** of the vulnerability
2. **Steps to reproduce** the issue
3. **Potential impact** of the vulnerability
4. **Affected versions** (if known)
5. **Suggested fix** (if you have one)
6. **Your contact information** for follow-up

### What to Expect

- **Acknowledgment**: We will acknowledge receipt of your report within 48 hours
- **Assessment**: We will assess the vulnerability and determine its severity within 5 business days
- **Updates**: We will keep you informed of our progress
- **Disclosure**: We will coordinate with you on public disclosure timing
- **Credit**: We will credit you in our security advisories (unless you prefer to remain anonymous)

## Security Best Practices

When using the SQLite Graph Database Extension:

### 1. Input Validation
- Always validate and sanitize user input before using it in Cypher queries
- Use parameterized queries when possible
- Avoid constructing queries from untrusted input

### 2. Access Control
- Run SQLite with appropriate file permissions
- Limit who can load extensions in your SQLite instance
- Use SQLite's built-in access control features

### 3. Extension Loading
- Only load the extension from trusted sources
- Verify checksums of downloaded binaries (see releases page)
- Keep the extension updated to the latest version

### 4. Memory Safety
- This is a native C extension that runs in the same process as SQLite
- While we use memory-safe practices, vulnerabilities may exist in alpha releases
- Consider running in isolated environments for untrusted workloads

### 5. Data Security
- The extension operates on in-memory data structures
- Sensitive data should be encrypted at rest using SQLite's encryption extensions
- Be aware that graph data may be exposed through error messages or logs

## Known Security Considerations

### Alpha Release Status
- This is an alpha release and has not undergone extensive security auditing
- The API and internal structures may change, potentially affecting security properties
- Use in production environments is not recommended at this time

### Memory Management
- The extension uses SQLite's memory allocation functions
- Memory leaks or use-after-free vulnerabilities may exist in alpha code
- We actively test with AddressSanitizer and Valgrind

### Extension Privileges
- SQLite extensions run with full process privileges
- Malicious queries could potentially exploit bugs to execute arbitrary code
- Only allow trusted users to submit Cypher queries

### Denial of Service
- Complex graph queries may consume significant CPU and memory
- Consider implementing query timeouts and resource limits
- Monitor for resource exhaustion attacks

## Security Update Policy

### Release Process
1. Security fixes will be released as patch versions (e.g., 0.1.1)
2. Critical vulnerabilities will be addressed with emergency releases
3. Security advisories will be published on GitHub
4. CVEs will be requested for significant vulnerabilities

### Notification Channels
- GitHub Security Advisories
- Release notes and CHANGELOG.md
- Email notifications to security@agentflare.ai subscribers
- Social media announcements for critical issues

## Vulnerability Disclosure Timeline

1. **Day 0**: Vulnerability reported privately
2. **Day 1-2**: Acknowledgment sent to reporter
3. **Day 1-7**: Vulnerability assessed and triaged
4. **Day 7-30**: Fix developed and tested
5. **Day 30**: Coordinated public disclosure (or earlier if actively exploited)

We follow responsible disclosure practices and appreciate security researchers who do the same.

## Security Hall of Fame

We will acknowledge security researchers who responsibly disclose vulnerabilities:

- *No reports yet - be the first!*

## Additional Resources

- [SQLite Security Documentation](https://www.sqlite.org/security.html)
- [OWASP SQL Injection Prevention](https://cheatsheetseries.owasp.org/cheatsheets/SQL_Injection_Prevention_Cheat_Sheet.html)
- [CWE Database](https://cwe.mitre.org/)

## Contact

For security-related questions that are not vulnerabilities, you can:
- Open a GitHub Discussion
- Email: security@agentflare.ai
- Review our [Contributing Guide](CONTRIBUTING.md)

---

**Last Updated**: 2025-10-24
**Version**: 0.1.0-alpha.0
