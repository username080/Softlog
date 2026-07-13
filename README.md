# Softlog

**Softlog** is a highly efficient, single-threaded, event-driven log monitoring daemon for Linux. It watches a specified log file and instantly sends a JSON webhook to platforms like Discord or Slack when a log line matches a defined Regular Expression.

Unlike standard polling scripts that waste CPU cycles, Softlog is built entirely on the Linux kernel's `inotify` subsystem. It perfectly sleeps at 0.0% CPU until the exact millisecond a file is modified.

## Features

- **Event-Driven Architecture:** Uses native `inotify` file-watching instead of CPU-heavy `while true; do sleep; done` polling.
- **Log Rotation Resilience:** Transparently handles the log file being deleted, moved, or swapped by `logrotate` without crashing or skipping a beat.
- **POSIX Regular Expressions:** Filter logs using standard Regex to capture exact error patterns while ignoring irrelevant warnings.
- **Robust Edge-Case Handling:** Safely escapes complex log lines (quotes, backslashes) to prevent JSON payload corruption and intelligently mitigates buffer overflows for massive log lines.
- **Native Systemd Integration:** Installs directly as a background Linux service.

## Requirements
Softlog is written in C and is extremely lightweight. It requires:
- `gcc` and `make` (for compiling)
- `libcurl` (for handling HTTPS webhooks)

*On Debian/Ubuntu:*
```bash
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev
```

## Installation

You can install Softlog as a native system daemon by running the provided install script:

```bash
git clone https://github.com/yourusername/softlog.git
cd softlog
sudo ./install.sh
```

## Configuration

After installation, configure the service to watch your specific log file and point to your webhook:

1. Edit the systemd service file:
   ```bash
   sudo nano /etc/systemd/system/softlog.service
   ```
2. Modify the `ExecStart` line with your parameters:
   ```ini
   ExecStart=/usr/local/bin/softlog -f /var/log/syslog -k "^FATAL:.*" -w "https://discord.com/api/webhooks/..."
   ```
3. Reload systemd and start the service:
   ```bash
   sudo systemctl daemon-reload
   sudo systemctl start softlog
   sudo systemctl enable softlog
   ```

## Usage (Standalone)

If you prefer to run it manually rather than as a system service, you can run the compiled binary directly:

```bash
./softlog -f <log_file> -k <regex_pattern> -w <webhook_url>
```

**Example:**
```bash
./softlog -f /var/log/nginx/error.log -k "^\[emerg\].*" -w "https://discord.com/api/webhooks/123/abc"
```

## Running the Unit Tests
Softlog comes with a zero-dependency C test suite to ensure the regex matcher and JSON escaping utilities are functioning perfectly.

```bash
make test
```

## License
MIT License
