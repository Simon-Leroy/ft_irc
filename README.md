*This project has been created as part of the 42 curriculum by alandel, sleroy*

# IRC: Internet Relay Chat Server

## Description

This project implements a functional IRC (Internet Relay Chat) server written in C++ 98. The server handles multiple concurrent clients through a non-blocking I/O architecture with a single polling mechanism, enabling real-time bidirectional communication between users.

### Goal

Develop a fully operational IRC server capable of supporting essential chat features including user authentication, channel management, and operator-specific commands, while managing multiple simultaneous connections efficiently without blocking operations.

### Transport Layer

The server communicates via **TCP/IP** (IPv4/IPv6) protocol. Connection handling is implemented through:
- **socket()** - Creates TCP endpoints for client connections
- **bind()** - Associates the server socket to a listening port
- **listen()** - Marks the socket as accepting incoming connections
- **accept()** - Accepts new client connections and returns unique file descriptors
- **connect()** - Establishes outbound connections (for client requests)
- **send/recv()** - Handles non-blocking data transmission
- **setsockopt()** - Configures socket options (SO_REUSEADDR, etc.)
- **htons** - Converts between host and network byte order
- **poll()** - Single polling mechanism monitoring all file descriptors for read/write events
- **fcntl()** - Sets non-blocking mode on file descriptors (required for macOS compatibility)

#### sockaddr_in Structure
```cpp
struct sockaddr_in {
    short sin_family;           // AF_INET (IPv4)
    unsigned short sin_port;    // Port number (in network byte order via htons)
    struct in_addr sin_addr;    // IP address
    char sin_zero[8];           // Padding (set to zero)
};

struct in_addr {
    unsigned long s_addr;       // 32-bit IPv4 address (network byte order)
};
```
Used to store server and client address information. Fields must be set in network byte order using conversion functions like `htons()` for ports.

#### pollfd Structure
```cpp
struct pollfd {
    int fd;                     // File descriptor to monitor
    short events;               // Events to monitor (POLLIN, POLLOUT, POLLERR)
    short revents;              // Events that occurred (set by poll())
};
```
Common flags:
- `POLLIN` - Data available to read
- `POLLOUT` - Ready to write data
- `POLLERR` - Error condition
- `POLLHUP` - Connection closed

One `pollfd` array entry per monitored file descriptor (listening socket + each client connection).

All I/O operations remain **non-blocking**, preventing server hangs while handling multiple clients simultaneously.

### IRC Protocol Implementation

The server implements core IRC functionality as specified in RFC 1459 and related RFCs. Key protocol features include:

**Authentication & User Management:**
- **PASS** - Authenticate with server password before connection
- **NICK** - Set or change client nickname
- **USER** - Register username and real name
- Validation of credentials before allowing channel/message operations

**Channel Operations:**
- **JOIN** - Subscribe to a channel and receive all channel messages
- **PART** - Leave a channel

**Private Messaging:**
- **PRIVMSG** - Send direct messages to specific users

**Channel Operator Commands:**
- **KICK** - Remove a client from the channel (operators only)
- **INVITE** - Invite a client to a channel (operators only)
- **TOPIC** - View or change the channel topic (restricted if mode +t)
- **MODE** - Manage channel modes:
  - `+/-i` - Invite-only channel
  - `+/-t` - Restrict TOPIC command to operators
  - `+/-k` - Set/remove channel password
  - `+/-o` - Grant/revoke operator privileges
  - `+/-l` - Set/remove maximum user limit

**Server Management:**
- **QUIT** - Disconnect cleanly from server
- **PING/PONG** - Keep-alive mechanism

## Instructions

### Compilation

```bash
make
```

Generates the executable `ircserv` using the provided Makefile.

Clean build:
```bash
make clean   # Remove object files
make fclean  # Remove all build artifacts
make re      # Clean rebuild
```

### Execution

**Start the server:**
```bash
./ircserv <port> <password>
```

Example:
```bash
./ircserv 6667 mypassword123
```

**Parameters:**
- `<port>` - TCP port number for listening (1024-65535 recommended; use 6667-6669 for standard IRC)
- `<password>` - Connection password required for all clients

### Using an IRC Client

**Option 1: IRSSI (Full-featured IRC client)**

Install (Debian/Ubuntu):
```bash
sudo apt install irssi
```

Launch and connect to your server:
```bash
irssi
```

Inside irssi, execute these commands in order:

```
# Connect to your server with password authentication
/connect localhost <port> <password>

# Set your nickname
/nick your_nickname

# Join a channel
/join #channelname

# Send a message in the channel
/msg #channelname Hello everyone!

# Send a private message to another user
/msg other_user Private message here

# Leave the channel
/part #channelname

# Disconnect
/quit
```

**Option 2: Netcat (Simple TCP connection)**
```bash
nc -C 127.0.0.1 <port>
```

Then manually type IRC commands (requires knowledge of IRC protocol).

### Available Commands

**Connection:**
- `PASS <password>` - Authenticate
- `NICK <nickname>` - Set nickname
- `USER <username> 0 * :<realname>` - Register user

**Channels:**
- `JOIN <#channel>` - Join a channel
- `PART <#channel>` - Leave a channel
- `TOPIC <#channel> [new topic]` - View/set topic
- `KICK <#channel> <nick>` - Remove user
- `INVITE <nick> <#channel>` - Invite user
- `MODE <#channel> <+/-modes> [parameters]` - Manage modes

**Messaging:**
- `PRIVMSG <target> :<message>` - Send private message
- `NOTICE <target> :<message>` - Send notice

**Server:**
- `QUIT [reason]` - Disconnect gracefully
- `PING` - Server keep-alive
- `LIST` - Show channels

## Resources

### Official References
- [RFC 1459 - Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459) - Core IRC specification
- [RFC 2811-2813 - IRC Architecture and Extensions](https://tools.ietf.org/html/rfc2811) - Advanced features and modes

### Technical Documentation
- [Linux man pages: socket API](https://man7.org/linux/man-pages/man7/socket.7.html) - Socket programming guide
- [POSIX poll() specification](https://pubs.opengroup.org/onlinepubs/9699919799/functions/poll.html) - Event polling mechanism
- [IPv4/IPv6 address handling](https://man7.org/linux/man-pages/man3/inet_aton.3.html) - Network byte order and conversions

### Learning Resources
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) - Comprehensive networking fundamentals
- [cppreference.com C++98](https://en.cppreference.com/) - C++ standard library reference
- [IRC Client Examples](https://libera.chat/) - Testing with real IRC networks (after server validation)

## Technical Notes

### Non-Blocking Architecture
The server uses a single `poll()` call to monitor all file descriptors (listening socket, client connections). This design:
- Prevents individual client operations from blocking others
- Scales efficiently with moderate connection counts
- Minimizes system resource consumption
- Ensures responsive multi-client handling

### Packet Aggregation
Commands may arrive fragmented across multiple packets. The server buffers incomplete commands until a complete line (terminated by `\r\n`) is received before parsing.

### Compiler & Standards
- **Language:** C++ 98
- **External libraries:** None (only POSIX/BSD socket APIs)
- **Compatibility:** Linux, BSD, macOS

## AI Usage

AI was utilized in this project for:

- **Protocol Understanding:** Clarifying RFC 1459 specifications and IRC command semantics
- **Non-blocking I/O Concepts:** Explaining polling mechanisms, event-driven architecture, and asynchronous handling
- **Socket Programming:** Best practices for TCP connection management and byte order conversions

---

**Last Updated:** March 2026