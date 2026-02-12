class StagingArea {
    public:
        void stageFile(const std::string& filename);
        void clear();
        std::vector<std::string> getStagedFiles() const;
    
    private:
        std::string stagingPath = ".mygit/staging";
};
    