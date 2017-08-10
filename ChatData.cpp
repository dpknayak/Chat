#include "ChatData.h"
#include <cstring>
#include <unistd.h>
ChatData::ChatData() {
}
ChatData::ChatData(int id, char* buffer, int len) {
    mId = id;
    mText = new char[1024];
    memset(mText, 0, 1024);
    strncpy(mText, buffer, len);
    
}
ChatData::~ChatData() {
    if (mText != NULL)
        delete mText;
}
