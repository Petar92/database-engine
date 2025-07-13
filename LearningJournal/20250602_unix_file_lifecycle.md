# Journal Entry - June 2nd, 2025
I have been learning a lot about the Unix file lifecycle as a part of my journey to understand process creation and inter-process communication, so I want to summarize the key concepts and steps involved in creating, managing, and deleting files in a Unix-like operating system. This will help me understand how files are handled at a low level, which is crucial for my ongoing projects.
# 🧱 FULL STEP-BY-STEP: From File Creation to Deletion

## ✅ Step 1: Create a new file
```bash
echo "example" > a.txt
```
**System call:** `open("a.txt", O_CREAT | O_WRONLY | O_TRUNC)` followed by `write(...)` and `close(...)`

**What happens:**
- ✅ **Inode is created**
  - Stores metadata (permissions, timestamps, size = 7 bytes, block pointers)
- ✅ **Directory entry is created**
  - Maps "a.txt" → inode number i12345
- ✅ **Inode link count = 1**
  - Because 1 directory entry (a.txt) points to it
- ✅ **File is opened, written to, and closed** — no open file descriptions left now

## ✅ Step 2: Create a hard link
```bash
ln a.txt b.txt
```
**What happens:**
- ✅ **New directory entry b.txt is created**, pointing to same inode as a.txt
- ✅ **Inode link count = 2**
  - Because both "a.txt" and "b.txt" point to the same inode
- **No new inode or file content** — both names point to the same data.

## ✅ Step 3: Open a.txt
```c
int fd = open("a.txt", O_WRONLY);
```
**What happens:**
- ✅ **Open file description (OFD) is created** in kernel-wide open file table
  - Stores: current offset = 0, flags = O_WRONLY, pointer to inode
- ✅ **Process FD table gets new entry** (e.g., fd = 3) pointing to this OFD
- ⚠️ **Inode link count stays at 2**
  - Because opening does not affect links — only the reference count on the OFD is incremented

## ✅ Step 4: Unlink a.txt
```bash
unlink("a.txt");
```
**What happens:**
- ✅ **Directory entry a.txt is removed**
- ✅ **Inode link count = 1**
  - Now only b.txt points to it
- ❗ **File is not deleted because:**
  - Inode still has 1 link (b.txt)
  - File is still open via fd

## ✅ Step 5: Write to the file
```c
write(fd, "hello", 5);
```
**What happens:**
- ✅ **Data is written** starting at offset 0
- ✅ **File offset in OFD is updated** to 5
- ✅ **File now contains "hellole"** (first 5 bytes overwritten)

## ✅ Step 6: Duplicate the FD
```c
int fd2 = dup(fd);
```
**What happens:**
- ✅ **New FD is created** in process FD table (e.g., fd2 = 4)
- ✅ **Points to same OFD**
- ✅ **File offset and flags are shared** between fd and fd2

## ✅ Step 7: Close both FDs
```c
close(fd);
close(fd2);
```
**What happens:**
- ✅ **Both FDs are removed** from process's FD table
- ✅ **Kernel sees that no FDs reference the OFD**
- ✅ **OFD is destroyed**

**BUT:**
- ⚠️ **Inode still has link count = 1** (b.txt)
- ⚠️ **File still exists on disk**

## ✅ Step 8: Remove b.txt
```bash
rm b.txt
```
**What happens:**
- ✅ **Directory entry b.txt is deleted**
- ✅ **Inode link count = 0**
- ✅ **No open FDs, so:**
  - Kernel frees inode
  - File contents (data blocks) are deleted

## ✅ Summary Table

| Structure | Created/Updated at | Deleted when |
|-----------|-------------------|--------------|
| Inode | File creation | Link count = 0 AND no open FDs |
| Directory entry | File creation, ln | unlink() or rm |
| Open file description | open() | All referring FDs are closed |
| File descriptor (FD) | open(), dup() | close(fd) |
| Process FD table | Per process, dynamic | On close(fd) or process exit |