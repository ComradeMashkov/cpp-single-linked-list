#pragma once

#include <algorithm>
#include <utility>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };
    
    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;
        
        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node)
            : node_(node) {}
        
    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора
        
        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;
        
        BasicIterator() = default;
        
        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept
            : node_(other.node_) {}
        
        BasicIterator& operator=(const BasicIterator& rhs) = default;
        
        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
        
        [[nodiscard]] bool operator !=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }
        
        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
        
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }
        
        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        BasicIterator& operator++() noexcept {
            node_ = node_->next_node;
            return *this;
        }
            
        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        BasicIterator operator++(int) noexcept {
            auto old_value(*this);
            ++(*this);
            return old_value;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }
        
        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        [[nodiscard]] pointer operator->() const noexcept {
            return (node_ != nullptr) ? &node_->value : nullptr;
        }
        
    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    
    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() {
    
    }
    
    template <typename IteratorType>
    void Init(IteratorType range_begin, IteratorType range_end) {
        Node* node = &head_;
        for (auto it = range_begin; it != range_end; ++it) {
            ++size_;
            node->next_node = new Node(*it, nullptr);
            node = node->next_node;
        }
    }
    
    SingleLinkedList(std::initializer_list<Type> values) {
        SingleLinkedList tmp;
        tmp.Init(values.begin(), values.end());
        swap(tmp);
    }
    
    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList tmp;
        tmp.Init(other.begin(), other.end());
        swap(tmp);
    }
    
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList tmp(rhs);
            swap(tmp);
        }
        return *this;
    }
    
    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }
    
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }
    
    /*
     * Вставляет элемент value после элемента, на который указывает pos
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
    */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator{pos.node_->next_node};
    }
    
    void PopFront() noexcept {
        Node* tmp = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = tmp;
        --size_;
    }
    
    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
    */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* tmp = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = tmp;
        --size_;
        return Iterator{pos.node_->next_node};
    }
    
    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* new_head = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = new_head;
        }
        size_ = 0u;
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return !size_;
    }
    
    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }
    
    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }
    
    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    
    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{nullptr};
    }
    
    // Возвращает константный итератор, ссылающийся на первый элемент
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    
    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }
    
    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }
    
    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }
    
    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }
    
    ~SingleLinkedList() {
        Clear();
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_ = 0u;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}