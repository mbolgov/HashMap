#include <iostream>
#include <list>
#include <vector>
#include <memory>
#include <stdexcept>

const double LOAD_FACTOR = 0.57;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    using SizeType = size_t;
    using ObjType = std::pair<const KeyType, ValueType>;
    using iterator = typename std::list<ObjType>::iterator;
    using const_iterator = typename std::list<ObjType>::const_iterator;

    HashMap(Hash hash = Hash()) : hash_(hash) {
        size_ = 0, capacity_ = 1, deleted_ = 0;
        map_.clear(), map_.resize(1), del_ = used_ = {false};
    }

    HashMap(iterator begin, iterator end, Hash hash = Hash()) : hash_(hash) {
        size_ = 0, capacity_ = 1, deleted_ = 0;
        map_.clear(), map_.resize(1), del_ = used_ = {false};
        while (begin != end) {
            insert(*(begin++));
        }
    }

    HashMap(std::initializer_list<ObjType> list, Hash hash = Hash()) : hash_(hash) {
        size_ = 0, capacity_ = 1, deleted_ = 0;
        map_.clear(), map_.resize(1), del_ = used_ = {false};
        for (auto &el: list) {
            insert(el);
        }
    }

    HashMap(const HashMap<KeyType, ValueType, Hash> &map) {
        hash_ = map.hash_;
        clear();
        for (auto &el: map) {
            insert(el);
        }
    }

    HashMap &operator=(HashMap<KeyType, ValueType, Hash> map) {
        hash_ = map.hash_;
        clear();
        for (auto &el: map) {
            insert(el);
        }
        return *this;
    }

    SizeType size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    Hash hash_function() const {
        return hash_;
    }

    void insert(const ObjType &p) {
        if (find(p.first) != list_.end()) {
            return;
        }
        list_.push_back(p);
        iterator obj = --list_.end();

        SizeType k = hash_(p.first) % capacity_, i = k;
        while (used_[i] && !del_[i]) {
            SizeType j = hash_(map_[i]->first) % capacity_;
            if ((i + capacity_ - k) % capacity_ < (i + capacity_ - j) % capacity_) {
                std::swap(obj, map_[i]), std::swap(k, j);
            }
            i = i == capacity_ - 1 ? 0 : i + 1;
        }
        if (del_[i]) {
            --deleted_;
        }
        map_[i] = obj, del_[i] = false, used_[i] = true, ++size_;

        i = i == capacity_ - 1 ? 0 : i + 1;
        while (used_[i] && !del_[i] && map_[i] != obj) {
            SizeType j = hash_(map_[i]->first) % capacity_;
            if ((i + capacity_ - k) % capacity_ < (i + capacity_ - j) % capacity_) {
                std::swap(obj, map_[i]), std::swap(k, j);
            }
            i = i == capacity_ - 1 ? 0 : i + 1;
        }

        if (size_ + deleted_ > LOAD_FACTOR * capacity_) {
            std::list<ObjType> tmp = list_;
            SizeType old_capacity = capacity_;
            clear();
            capacity_ = old_capacity * 2;
            map_.clear(), map_.resize(capacity_), del_.assign(capacity_, false), used_.assign(capacity_, false);
            for (auto &tmp_obj: tmp) {
                insert(tmp_obj);
            }
        }
    }

    void erase(const KeyType &key) {
        for (SizeType i = hash_(key) % capacity_; used_[i]; i = i == capacity_ - 1 ? 0 : i + 1) {
            if (map_[i]->first == key) {
                list_.erase(map_[i]), del_[i] = true, ++deleted_, --size_;
                break;
            }
        }
        if (deleted_ > size_) {
            std::list<ObjType> tmp;
            for (SizeType i = 0; i < capacity_; ++i) {
                if (used_[i] && !del_[i]) {
                    tmp.push_back(*map_[i]);
                }
            }
            SizeType old_capacity = capacity_;
            clear();
            capacity_ = old_capacity;
            map_.clear(), map_.resize(capacity_), del_.assign(capacity_, false), used_.assign(capacity_, false);
            for (auto &tmp_obj: tmp) {
                insert(tmp_obj);
            }
        }
    }

    iterator find(const KeyType &key) {
        for (SizeType i = hash_(key) % capacity_; used_[i]; i = i == capacity_ - 1 ? 0 : i + 1) {
            if (map_[i]->first == key) {
                if (del_[i]) {
                    return list_.end();
                }
                return map_[i];
            }
        }
        return list_.end();
    }

    const_iterator find(const KeyType &key) const {
        for (SizeType i = hash_(key) % capacity_; used_[i]; i = i == capacity_ - 1 ? 0 : i + 1) {
            if (map_[i]->first == key) {
                if (del_[i]) {
                    return list_.end();
                }
                return map_[i];
            }
        }
        return list_.end();
    }

    ValueType &operator[](const KeyType &key) {
        auto it = find(key);
        if (it == list_.end()) {
            insert({key, ValueType()});
            it = find(key);
        }
        return it->second;
    }

    const ValueType &at(const KeyType &key) const {
        auto it = find(key);
        if (it == list_.end()) {
            throw std::out_of_range("");
        }
        return it->second;
    }

    void clear() {
        size_ = 0, capacity_ = 1, deleted_ = 0;
        list_.clear(), map_.clear(), map_.resize(capacity_), del_ = used_ = {false};
    }

    iterator begin() {
        return list_.begin();
    }

    iterator end() {
        return list_.end();
    }

    const_iterator begin() const {
        return list_.begin();
    }

    const_iterator end() const {
        return list_.end();
    }

protected:
    Hash hash_;
    SizeType size_, capacity_, deleted_;
    std::list<ObjType> list_;
    std::vector<iterator> map_;
    std::vector<bool> del_, used_;
};
