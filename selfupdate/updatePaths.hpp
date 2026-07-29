#pragma once

#include <string>

class PathsUpdater {
    private:
        std::string HOME_PATH;
        std::string LOG_PATH_TO_UPDATE;
        std::string TEMP_PATH;
        std::string PATH_TO_SELF;

        void setHomePath();
        void setLogPathToUpdate();
        void setTempPath();
        void setPathToSelf();
        void setAllPaths();
    public:
        PathsUpdater() { setAllPaths(); }
        std::string getHomePath();
        std::string getLogPathToUpdate();
        std::string getTempPath();
        std::string getPathToSelf();
};
