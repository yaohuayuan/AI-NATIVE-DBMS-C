# Testing

Default tests are local and deterministic.

Rules for v0.1:

- No network access in default CTest.
- No real API calls in default CTest.
- No dependency on API keys in default CTest.
- Manual real API tests belong under `tests/manual`.
