# 🖥️ Multithreaded C++ HTTP Server & Finance Tracker

its a custom, lightweight, multithreaded HTTP/1.1 Web Server written in C++17 from scratch using POSIX sockets. It includes a thread pool for handling client connections concurrently, a robust HTTP request parser, validation middleware, and a regular expression-based router.

The server hosts a REST API for a **Finance Tracker** and serves its static web frontend (HTML, CSS, JS) from a `public/` directory.

---

## 🚀 Key Features

- **Multi-threaded Connection Engine:** Handles client connections concurrently using a pre-allocated pool of worker threads synchronized with `std::mutex` and `std::condition_variable`.
- **Custom HTTP/1.1 Parser & Builder:** Parses HTTP start lines, headers, and body payloads using `Content-Length` header. Formats compliant HTTP/1.1 response packages.
- **HTTP Request Validation:** Enforces HTTP specifications such as valid HTTP/1.1 versions, supported HTTP methods (GET, POST, DELETE, PUT, HEAD), the presence of the `Host` header, and validation of header formatting.
- **Regex-based Router:** Dynamically maps route rules (HTTP methods + route patterns) to functional handler targets, extracting parameter parameters (like transaction IDs) automatically.
- **Static File Server:** Inspects requested routes and serves static pages (`.html`, `.css`, `.js`, images, etc.) from the public folder, matching the correct MIME types.
- **Thread-safe Transaction Store:** Thread-safe state storage (`std::vector`) guarded by transaction mutex locks to support reading, creating, and deleting records dynamically.

---

## 🛠️ Tech Stack

- **Backend:** C++17, POSIX Sockets, POSIX Threads (`std::thread`), `nlohmann/json`
- **Frontend:** HTML5, CSS3 (Custom responsive styles), JavaScript (ES6 Fetch API)
- **Build System:** GNU Make

---

## ⚙️ REST API Specification

### 1. Retrieve Transactions
- **Path:** `/transactions`
- **Method:** `GET`
- **Response:** `200 OK` with JSON array.
- **Example Payload:**
  ```json
  [
    {"id": 1, "description": "Salary", "amount": 5000.00},
    {"id": 2, "description": "Rent", "amount": -1200.00}
  ]
  ```

### 2. Create Transaction
- **Path:** `/transactions`
- **Method:** `POST`
- **Headers:** `Content-Type: application/json`
- **Request Body:**
  ```json
  {"description": "Freelance", "amount": 850.50}
  ```
- **Response:** `204 No Content`

### 3. Delete Transaction
- **Path:** `/transactions/<id>`
- **Method:** `DELETE`
- **Response:** `200 OK`
- **Example Payload:**
  ```json
  {"message": "Deleted"}
  ```

---

## ⚡ Getting Started

### 📋 Prerequisites

Ensure you have a C++ compiler supporting standard C++17, as well as `nlohmann/json` development libraries.
On Debian/Ubuntu systems:
```bash
sudo apt-get install g++ libnlohmann-json-dev make
```

### 🔨 Compilation

To build both the `server` and the `client` binaries:
```bash
make all
```

To clear built executables:
```bash
make clean
```

### 🚀 Running the Server

Start the server binary by passing the **number of threads** to instantiate in the thread pool:
```bash
./server <number_of_threads>
```
For example, to run the server with a pool of 4 worker threads:
```bash
./server 4
```
The server will boot and begin listening on **`http://localhost:8080/`**.

### 💻 Running the Test Client

In a separate terminal, compile and run the CLI client:
```bash
./client
```
The client connects to the running server, sends a basic HTTP request, prints the server's output, and closes.
