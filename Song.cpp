#include "Song.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>

using namespace std;

// 匿名命名空间 - 实现辅助函数
namespace {

string trim_copy(const string &s) {
    auto first = find_if_not(s.begin(), s.end(), [](unsigned char ch) {
        return isspace(ch);
    });
    auto last = find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) {
        return isspace(ch);
    }).base();
    if (first >= last) return "";
    return string(first, last);
}

string to_lower_copy(const string &s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), [](unsigned char ch) {
        return tolower(ch);
    });
    return result;
}

string join_tags(const vector<string> &tags) {
    string result;
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) result += ", ";
        result += tags[i];
    }
    return result;
}

} // 匿名命名空间结束

// 静态成员初始化
int Song::next_id_ = 1;

// 构造函数
Song::Song(const string &title, const string &artist, int duration_sec, int rating) 
    : valid_(false) {
    
    string clean_title = trim_copy(title);
    string clean_artist = trim_copy(artist);
    
    // 数据验证
    if (clean_title.empty()) {
        cout << "[错误] 标题不能为空\n";
        return;
    }
    if (clean_artist.empty()) {
        cout << "[错误] 艺人不能为空\n";
        return;
    }
    if (duration_sec <= 0) {
        cout << "[错误] 时长必须为正整数（秒）\n";
        return;
    }
    if (rating < 1 || rating > 5) {
        cout << "[错误] 评分必须在 1...5 之间\n";
        return;
    }
    
    // 所有验证通过，设置成员变量
    id_ = next_id_++;
    title_ = clean_title;
    artist_ = clean_artist;
    duration_sec_ = duration_sec;
    rating_ = rating;
    valid_ = true;
}

// Setter 函数
bool Song::set_title(const string &t) {
    string clean_title = trim_copy(t);
    if (clean_title.empty()) {
        cout << "[提示] 标题不能为空，已忽略本次修改\n";
        return false;
    }
    title_ = clean_title;
    return true;
}

bool Song::set_artist(const string &a) {
    string clean_artist = trim_copy(a);
    if (clean_artist.empty()) {
        cout << "[提示] 艺人不能为空，已忽略本次修改\n";
        return false;
    }
    artist_ = clean_artist;
    return true;
}

bool Song::set_duration(int sec) {
    if (sec <= 0) {
        cout << "[提示] 时长需为正整数，已忽略本次修改\n";
        return false;
    }
    duration_sec_ = sec;
    return true;
}

bool Song::set_rating(int r) {
    if (r < 1 || r > 5) {
        cout << "[提示] 评分需在 1..5，已忽略本次修改\n";
        return false;
    }
    rating_ = r;
    return true;
}

// 标签管理
bool Song::add_tag(const string &tag) {
    string clean_tag = trim_copy(tag);
    if (clean_tag.empty()) {
        cout << "[提示] 空标签已忽略\n";
        return false;
    }
    
    string lower_tag = to_lower_copy(clean_tag);
    for (const auto &existing_tag : tags_) {
        if (to_lower_copy(existing_tag) == lower_tag) {
            cout << "[提示] 标签已存在（忽略大小写）\n";
            return false;
        }
    }
    
    tags_.push_back(clean_tag);
    return true;
}

bool Song::remove_tag(const string &tag) {
    string clean_tag = trim_copy(tag);
    string lower_tag = to_lower_copy(clean_tag);
    
    for (size_t i = 0; i < tags_.size(); ++i) {
        if (to_lower_copy(tags_[i]) == lower_tag) {
            tags_.erase(tags_.begin() + i);
            return true;
        }
    }
    
    cout << "[提示] 未找到该标签\n";
    return false;
}

// 关键词匹配
bool Song::matches_keyword(const string &kw) const {
    string clean_kw = trim_copy(kw);
    if (clean_kw.empty()) return false;
    
    string lower_kw = to_lower_copy(clean_kw);
    string lower_title = to_lower_copy(title_);
    string lower_artist = to_lower_copy(artist_);
    
    if (lower_title.find(lower_kw) != string::npos) return true;
    if (lower_artist.find(lower_kw) != string::npos) return true;
    
    for (const auto &tag : tags_) {
        if (to_lower_copy(tag).find(lower_kw) != string::npos) return true;
    }
    
    return false;
}

// 操作符重载
ostream &operator<<(ostream &os, const Song &s) {
    os << "[#" << s.id_ << "] " << s.artist_ << " - " << s.title_ 
       << " (" << s.duration_sec_ << "s) " << string(s.rating_, '*');
    
    if (!s.tags_.empty()) {
        os << " [tags: " << join_tags(s.tags_) << "]";
    }
    
    return os;
}

bool operator<(const Song &a, const Song &b) {
    // 1. 按评分降序
    if (a.rating_ > b.rating_) return true;
    if (a.rating_ < b.rating_) return false;
    
    // 2. 按标题升序
    if (a.title_ < b.title_) return true;
    if (a.title_ > b.title_) return false;
    
    // 3. 按ID升序
    return a.id_ < b.id_;
}