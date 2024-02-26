#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next) : value(val), next_node(next) {}
        
        Type value = Type();
        Node* next_node = nullptr;
    };
    
    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;
        
        BasicIterator() = default;
        explicit BasicIterator(Node* node) : node_(node) {}
        BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {}
        
        BasicIterator& operator=(const BasicIterator& rhs) = default;
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept { return node_ == rhs.node_; }
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept { return !(*this == rhs); }
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept { return node_ == rhs.node_; }
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept { return !(*this == rhs); }
        BasicIterator& operator++() noexcept { return node_ = node_->next_node, *this; }
        BasicIterator operator++(int) noexcept { auto temp(*this); return ++*this, temp; }
        [[nodiscard]] reference operator*() const noexcept { return node_->value; }
        [[nodiscard]] pointer operator->() const noexcept { return &node_->value; }
    private:
        Node* node_ = nullptr;
    };
public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;
    
    SingleLinkedList() = default;
    SingleLinkedList(std::initializer_list<Type> values) { BuildList(values); }
    SingleLinkedList(const SingleLinkedList& other) { BuildList(other); }
    
    ~SingleLinkedList() { Clear(); }
    
    SingleLinkedList& operator=(const SingleLinkedList& rhs) { return BuildList(rhs), *this; }
    bool operator==(const SingleLinkedList<Type>& rhs) const { return GetSize() == rhs.GetSize() && std::equal(begin(), end(), rhs.begin()); }
    bool operator!=(const SingleLinkedList<Type>& rhs) const { return !(*this == rhs); }
    bool operator<(const SingleLinkedList<Type>& rhs) const { return std::lexicographical_compare(begin(), end(), rhs.begin(), rhs.end()); }
    bool operator<=(const SingleLinkedList<Type>& rhs) const { return *this < rhs || *this == rhs; }
    bool operator>(const SingleLinkedList<Type>& rhs) const { return rhs < *this; }
    bool operator>=(const SingleLinkedList<Type>& rhs) const { return rhs < *this || rhs == *this; }
    
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }
    
    [[nodiscard]] bool IsEmpty() const noexcept {
        return !size_;
    }
    
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }
    
    void PopFront() noexcept {
        Node* front_node = head_.next_node;
        Node* next_front_node = front_node->next_node;
        
        delete front_node;
        head_.next_node = next_front_node;
        --size_;
    }
    
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* pos_after_insert = pos.node_->next_node;
        
        pos.node_->next_node = new Node(value, pos_after_insert);
        ++size_;
        
        return Iterator(pos.node_->next_node);
    }
    
    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* pos_to_remove = pos.node_->next_node;
        Node* pos_after_remove = pos_to_remove->next_node;
        
        delete pos_to_remove;
        pos.node_->next_node = pos_after_remove;
        --size_;
        
        return Iterator(pos_after_remove);
    }
    
    void Clear() noexcept {
        while (head_.next_node) {
            Node* node_after_next = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = node_after_next;
        }
        size_ = 0;
    }
    
    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }
    
    [[nodiscard]] Iterator begin() noexcept { return Iterator(head_.next_node); }
    [[nodiscard]] Iterator end() noexcept { return Iterator(FindListEnd()); }
    [[nodiscard]] ConstIterator begin() const noexcept { return ConstIterator(head_.next_node); }
    [[nodiscard]] ConstIterator end() const noexcept { return ConstIterator(FindListEnd()); }
    [[nodiscard]] ConstIterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] ConstIterator cend() const noexcept { return end(); }
    [[nodiscard]] Iterator before_begin() noexcept { return Iterator(&head_); }
    [[nodiscard]] ConstIterator before_begin() const noexcept { return ConstIterator(&const_cast<Node&>(head_)); }
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept { return before_begin(); }
private:
    template <typename Container>
    void BuildList(Container& c) {
        if (this == reinterpret_cast<const SingleLinkedList*>(&c)) {
            return;
        }
        
        SingleLinkedList temp;
        Node* curr_node = &temp.head_;        
        for (auto it = c.begin(); it != c.end(); ++it) {
            curr_node->next_node = new Node(*it, nullptr);
            curr_node = curr_node->next_node;
        }
        temp.size_ = std::distance(c.begin(), c.end());
        
        swap(temp);
    }
    
    Node* FindListEnd() const {
        Node* curr_node = head_.next_node;
        while (curr_node) {
            curr_node = curr_node->next_node;
        }
        return curr_node;
    }
    
    Node head_ = Node();
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}
/*
template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs < rhs || lhs == rhs;
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs || rhs == lhs;
}
*/
