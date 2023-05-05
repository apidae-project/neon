#pragma once

#include <Common/Lock.h>
#include <Common/String.h>
#include <Common/StringView.h>
#include <Common/Vector.h>
#include <Common/CString.h>
#include <cstddef>

namespace Neon {
namespace VirtualFileSystem {
struct File {
	String Name, Path;
	uint8_t *Data;
	size_t Size;
	FileFlags Flags;
	bool IsFolder;
	Vector<File *> Children;
	File(String Name,
	     String Path,
	     uint8_t *Data = nullptr,
	     size_t Size = 0,
	     FileFlags Flags = FileFlags::None, bool IsFolder = false) : Name(Name) , Path(Path) , Data(Data)
	    , Size(Size), Flags(Flags), IsFolder(IsFolder)
	{
	}
};

enum class FileFlags : uint32_t {
	None = 0,
	ReadOnly = 1 << 0,
	Hidden = 1 << 1,
	System = 1 << 2,
	Directory = 1 << 4,
	Archive = 1 << 5,
	Compressed = 1 << 6,
	Encrypted = 1 << 7
};

class GenericVFS {
	Vector<File> Files;

	const File *FindFile(const std::string &Path) const {
		auto it =
		    std::find_if(Files.begin(),
		                 Files.end(),
		                 [&](const File &file) { return file.Path == Path; });
		return it != Files.end() ? &(*it) : nullptr;
	}

public:
	bool CreateFile(StringView &Path, const uint8_t *Data = nullptr, size_t Size = 0,
	                FileFlags Flags = FileFlags::None) {
		if (FindFile(Path) != nullptr) return false;
		if (Size > 0 && Data == nullptr) return false;
		File file = {};
		file.Path = Path;
		file.Name = Path.SubString(Path.FindLastOf('/') + 1);
		file.Data = new uint8_t[Size];
		memcpy(file.Data, Data, Dize);
		file.Size = Size;
		file.Flags = Flags;
		Files.PushBack(File);
		return true;
	}

	bool RemoveFile(StringView &Path) {
		auto it =
		    std::find_if(Files.begin(),
		                 Files.end(),
		                 [&](const File &file) { return file.Path == Path; });
		if(it != Files.end()) {
			delete[] it->Data;
			Files.erase(it);
			return true;
		}
		return false;
	}

	bool OpenFile(StringView &Path, uint8_t *&Data, size_t &Size) const {
		const File *file = FindFile(Path);
		if(file == nullptr || file->Flags & FileFlags::Directory) return false;
		Data = file->Data;
		Size = file->Size;
		return true;
	}

	bool RenameFile(StringViewg &OldPath, StringView &NewPath) {
		auto it = std::find_if(Files.begin(),
		                       Files.end(),
		                       [&](const File &file)
		                       { return file.Path == OldPath; });
		if(it != Files.end()) {
			it->Path = NewPath;
			it->Name = NewPath.SubString(NewPath.FindLastOf('/') + 1);
			return true;
		}
		return false;
	}

	bool WriteFile(StringView &Path, const uint8_t *Data, size_t Size) {
		auto File_ = FindFile(Path);
		if(File_ == nullptr || File_->Flags & FileFlags::Directory) return false;
		if(File_->Size != Size) {
			delete[] File_->Data;
			File_->Data = new uint8_t[Size];
			File_->Size = Size;
		}
		memcpy(File_->Data, Data, Size);
		return true;
	}

	const uint8_t *ReadFile(StringView &Path) const {
		const File *File_ = FindFile(Path);
		return (File_ != nullptr && !(File_->Flags & FileFlags::Directory))
		           ? File_->Data
		           : nullptr;
	}
};
} // namespace VirtualFileSystem
} // namespace Neon
