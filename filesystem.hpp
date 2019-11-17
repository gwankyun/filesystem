#include <cstring>
#include <fstream>

typedef unsigned long long uintmax_t;

namespace filesystem
{
    struct space_info
    {
        uintmax_t capacity;
        uintmax_t free;
        uintmax_t available;
    };

    class path;
    bool is_directory(path path_);
    class path
    {
    public:
        path() : m_path(_T(""))
        {
        }

        path(CString path_) : m_path(path_)
        {
        }

        CString cstring()
        {
            return m_path;
        }

        path parent_path()
        {
            int e = m_path.ReverseFind(_T('\\'));
            return path(m_path.Mid(0, e));
        }

        path operator/(CString other)
        {
            CString str = cstring();
            if (is_directory(*this))
            {
                str.Append(_T("\\"));
            }
            str.Append(other);
            return path(str);
        }

        path filename()
        {
            int e = m_path.ReverseFind(_T('\\'));
            return path(m_path.Mid(e + 1));
        }

        path& replace_extension(path new_extension = path())
        {
            int e = m_path.ReverseFind(_T('.'));
            m_path = m_path.Mid(0, e);
            m_path.Append(new_extension.cstring());
            return *this;
        }

        operator CString()
        {
            return cstring();
        }

        operator LPCWSTR()
        {
            return cstring();
        }

    private:
        CString m_path;
    };

    inline bool is_directory(path path_)
    {
        WIN32_FILE_ATTRIBUTE_DATA data;
        GetFileAttributesEx(path_, GetFileExInfoStandard, &data);
        return data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    }

    inline bool exists(path path_)
    {
        return GetFileAttributes(path_) != INVALID_FILE_ATTRIBUTES;
    }

    inline uintmax_t file_size(path path_)
    {
        std::fstream file;
        file.open(path_.cstring(), std::ios::in | std::ios::binary | std::ios::_Nocreate);
        uintmax_t size = 0;
        if (file.is_open())
        {
            file.seekg(0, file.end);
            size = file.tellg();
        }
        return size;
    }

    inline space_info space(path path_)
    {
        uintmax_t freeBytesToCaller = 0;
        uintmax_t totalBytes = 0;
        uintmax_t freeBytes = 0;
        BOOL result = GetDiskFreeSpaceEx(path_,
            (PULARGE_INTEGER)&freeBytesToCaller,
            (PULARGE_INTEGER)&totalBytes,
            (PULARGE_INTEGER)&freeBytes);
        space_info info;
        info.capacity = totalBytes;
        info.free = freeBytesToCaller;
        info.available = freeBytes;
        return info;
    }

    inline bool create_directory(path path_)
    {
        return CreateDirectory(path_, NULL);
    }
}
