﻿# Journal Entry - June 2nd, 2025
Since I got confused learning about the things from the previous 2 entries and in order to better understand and have a clear overview of the Unix file system, I have created a dictionary that summarizes the key concepts and terms related to file descriptors, inodes, and file operations. This will help me as I continue to develop my process pool and inter-process communication mechanisms.
# 📘 Unix File System Dictionary

## 🔹 File Descriptor (FD)
- An integer returned by `open()` (e.g., 3)
- Index into the per-process FD table
- Used by syscalls: `read(fd, ...)`, `write(fd, ...)`, etc.

## 🔹 File Descriptor Table
- Per-process table that maps integers (FDs) → open file descriptions
- Modified by `open()`, `dup()`, `close()`
- Automatically destroyed on process exit

## 🔹 Open File Description (OFD)
- Kernel-wide structure created by `open()`
- **Contains:**
  - File offset
  - File access flags (e.g., `O_WRONLY`)
  - Pointer to inode
- Can be shared across processes or FDs (e.g., via `dup`, `fork`)

## 🔹 Inode
- Filesystem-level structure representing a file's metadata
- **Contains:**
  - File size
  - Owner UID/GID
  - Permissions
  - Timestamps
  - Pointers to data blocks
- Does **NOT** store filename
- Exists until link count and open-FD count reach 0

## 🔹 Directory Entry
- Maps a filename → inode number
- Created by `open(O_CREAT)` or `ln`
- Removed by `unlink()` or `rm`

## 🔹 Link Count
- Number of directory entries pointing to an inode
- `ln a.txt b.txt` → increases link count to 2
- `unlink()` or `rm` → decreases it
- **Inode is deleted when:**
  - Link count = 0
  - No FDs refer to it

## 🔹 Hard Link
- Another name for the same file (same inode)
- Created via `ln`
- Shares inode and data — equal peers

## 🔹 Symbolic Link (symlink)
- A separate file that contains a path to another file
- Not the same inode
- Broken if the target is deleted

## 🔹 dup / dup2 / dup3
- Create new file descriptors that point to the same open file description
- `dup(fd)` → next available FD
- `dup2(fd, 5)` → duplicate to FD 5
- They share offset and flags

## 🔹 unlink()
- Removes a directory entry
- Decrements inode link count
- **File is deleted when:**
  - Link count = 0
  - No open FDs refer to it

## 🔹 rm
- User-level command that calls `unlink()`