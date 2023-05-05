#pragma once

#include <Core/VirtualFileSystem.h>
#include <Common/Lock.h>
#include <atomic>

namespace Neon
{
namespace POSIX
{

struct Filesystem;
struct Resource
{
	std::atomic<size_t> ReferenceCount;
	bool Reference();
	bool UnReference();
	virtual ssize_t Read();
	virtual ssize_t Write();
};

class Node
{
	Node &InternalReduce(bool Symlinks, bool Automount, size_t Count);

public:
	lock_t Lock;
	String Name;
	String Target;
	Node &MountGate;
	Node &Parent;
	Filesystem *Fs;
	Resource *Res;
	~Node()
	{
		if (this->Res && this->Res->ReferenceCount == 0) delete this->Res;
	}
};

struct FileSystem
{
	Node *MountedOn = nullptr;
	void *MountData = nullptr;
	Node *Root = nullptr;
	virtual int ioctl(Node &Node, uint64_t Request, void *argp);
};

enum Types
{
	s_ifmt = 0170000,
	s_ifsock = 0140000,
	s_iflnk = 0120000,
	s_ifreg = 0100000,
	s_ifblk = 0060000,
	s_ifdir = 0040000,
	s_ifchr = 0020000,
	s_ififo = 0010000,
};

enum DeviceTypes
{
	dt_unknown = 0,
	dt_fifo = 1,
	dt_chr = 2,
	dt_dir = 4,
	dt_blk = 6,
	dt_reg = 8,
	dt_lnk = 10,
	dt_sock = 12,
	dt_wht = 14
};

enum Modes
{
	s_irwxu = 00700, // user rwx
	s_irusr = 00400, // user r
	s_iwusr = 00200, // user w
	s_ixusr = 00100, // user x
	s_irwxg = 00070, // group rwx
	s_irgrp = 00040, // group r
	s_iwgrp = 00020, // group w
	s_ixgrp = 00010, // group x
	s_irwxo = 00007, // others rwx
	s_iroth = 00004, // others r
	s_iwoth = 00002, // others w
	s_ixoth = 00001, // others x
	s_isuid = 04000, // set-user-id
	s_isgid = 02000, // set-group-id
	s_isvtx = 01000, // set-sticky
};

enum OFlags
{
	o_async = 020000,
	o_direct = 040000,
	o_largefile = 0100000,
	o_noatime = 01000000,
	o_path = 010000000,
	o_tmpfile = 020000000,
	o_exec = o_path,
	o_search = o_path,
	o_accmode = (03 | o_path),
	o_rdonly = 00,
	o_wronly = 01,
	o_rdwr = 02,
	o_creat = 0100,
	o_excl = 0200,
	o_noctty = 0400,
	o_trunc = 01000,
	o_append = 02000,
	o_nonblock = 04000,
	o_dsync = 010000,
	o_sync = 04010000,
	o_rsync = 04010000,
	o_directory = 0200000,
	o_nofollow = 0400000,
	o_cloexec = 02000000
};

enum ATFlags
{
	at_fdcwd = -100,
	at_symlink_follow = 0x400,
	at_symlink_nofollow = 0x100,
	at_removedir = 0x200,
	at_no_automount = 0x800,
	at_empty_path = 0x1000,
	at_eaccess = 0x200
};

struct Handle
{
	lock_t Lock;
	VirtualFileSystem::Node *Node;
	long Offset;
	std::atomic<size_t> ReferenceCount;
	VirtualFileSystem::Resource *Resource;
	int Flags;
	constexpr Handle(VirtualFileSystem::Resource *Resource_, int Flags)
	    : Lock()
	    , Node(nullptr)
	    , Resource(Resource_)
	    , ReferenceCount(1)
	    , Offset(0)
	    , Flags(0)
	{
	}
};

struct FileDescriptor
{
	Handle *Handle;
	int Flags;
};

struct Path
{
};
struct PathView
{
};
struct File
{
};

FileDescriptor *FileDescriptorToResource(VirtualFileSystem::Resource *Resource,
                                         int Flags);
} // namespace POSIX
} // namespace Neon
