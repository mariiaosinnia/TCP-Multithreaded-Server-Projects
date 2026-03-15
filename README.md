# PCSP Assignments 3 & 4

## Overview
This repository contains university assignments from the *Parallel and Client-Server Programming (PCSP)* course. It includes two main projects:

1. **Assignment 3 – TCP Client & Server**  
   A basic client-server application implemented in C++ using TCP sockets. The client can send requests to the server, and the server responds with appropriate messages or file contents.

2. **Assignment 4 – Multithreaded Server with Thread Pool**  
   An extension of Assignment 3 that supports multiple concurrent clients using a **thread pool** architecture. The server maintains compatibility with clients from Assignment 3 while efficiently handling multiple simultaneous connections.  
   **Protocol update:** in Assignment 4, the client sends its name at the beginning of the request and the server creates a dedicated folder for that client.

## Project Structure

   ```
   PCSP_Assignments_3_4_6/
   │
   ├─ Assignment_3/ # TCP client-server project
   ├─ Assignment_4/ # Multithreaded server with thread pool
   ├─ Shared/ # Shared code for both assignments
   ├─ x64/
   ├─ PCSP_Assignments_3_4_6.sln
   └─ .gitignore
   ```


## Features

### Assignment 3
- TCP client-server communication
- File transfer from client to server
- Basic request handling

### Assignment 4
- Multithreaded server using **thread pool**
- Efficient handling of multiple simultaneous clients
- **Protocol update:** client sends its name at the beginning
- Server creates a dedicated folder for each client
- Backwards-compatible with Assignment 3 clients
- Thread-safe resource sharing

## Supported Commands

The client can send the following commands to the server:

- `LIST` – list available files on the server  
- `GET <filename>` – download a file from the server  
- `PUT <filename>` – upload a file to the server
- `DELETE <filename>` – delete a file from the server  
- `INFO <filename>` – retrieve information about a file

## Notes
- Shared code resides in the `Shared` project to avoid duplication.
