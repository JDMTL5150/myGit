class FileManager {
    public:
        static void copyFile(const std::string& from, const std::string& to);
        static void createDirectory(const std::string& path);
        static bool exists(const std::string& path);
};