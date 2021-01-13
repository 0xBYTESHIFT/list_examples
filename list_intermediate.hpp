#pragma once

namespace intermediate {

    template<class T>
    class list {
        struct node {
            T* val;
            node* next;

            node() {
                val = nullptr;
            }
            node(const T& val) {
                this->val = new T(val);
            }
            ~node() {
                if (val)
                    delete val;
            }
        };
        node *m_begin, *m_pre_end, *m_end;
        size_t m_size;

        void p_init();
        node* p_get_before(const node* n) const;

    public:
        class iterator {
            friend class intermediate::list<T>;
            node* m_node;

            iterator(node* n);

        public:
            iterator();
            iterator(const iterator& it);
            iterator(iterator&& it);
            ~iterator();

            T& operator*();
            const T& operator*() const;
            iterator& operator=(const iterator& rhs);
            iterator& operator++();
            iterator operator++(int);
            iterator operator+(size_t i) const;
            bool operator==(const iterator& it) const;
            bool operator!=(const iterator& it) const;
        };

        list(size_t size = 0);
        list(const list& rhs);
        list(list&& rhs);
        ~list();

        size_t size() const;
        bool empty() const;
        void clear();

        iterator begin() const;
        iterator end() const;
        iterator emplace_after(const iterator& it, const T& val);
        iterator emplace_back(const T& val);
        iterator erase(const iterator& it);
    };

    //iterator
    template<class T>
    list<T>::iterator::iterator() {}

    template<class T>
    list<T>::iterator::~iterator() {}

    template<class T>
    list<T>::iterator::iterator(node* n) {
        this->m_node = n;
    }

    template<class T>
    list<T>::iterator::iterator(const iterator& it) {
        this->m_node = it.m_node;
    }

    template<class T>
    list<T>::iterator::iterator(iterator&& it) {
        this->m_node = std::move(it.m_node);
        it.m_node = nullptr;
    }

    template<class T>
    T& list<T>::iterator::operator*() { return *m_node->val; }

    template<class T>
    const T& list<T>::iterator::operator*() const { return *m_node->val; }

    template<class T>
    typename list<T>::iterator& list<T>::iterator::operator=(const iterator& rhs) {
        this->m_node = rhs.m_node;
        return *this;
    }

    template<class T>
    typename list<T>::iterator& list<T>::iterator::operator++() {
        this->m_node = this->m_node->next;
        return *this;
    }

    template<class T>
    typename list<T>::iterator list<T>::iterator::operator++(int) {
        auto copy = *this;
        ++(*this);
        return copy;
    }

    template<class T>
    typename list<T>::iterator list<T>::iterator::operator+(size_t i) const {
        iterator bak(*this);
        while (i > 0) {
            bak++;
            i--;
        }
        return bak;
    }

    template<class T>
    bool list<T>::iterator::operator==(const iterator& it) const {
        return this->m_node == it.m_node;
    }

    template<class T>
    bool list<T>::iterator::operator!=(const iterator& it) const {
        return !(it == *this);
    }

    //list
    template<class T>
    list<T>::list(size_t size) {
        p_init();
        this->m_size = size;
        auto bak = begin();
        while (size > 0) {
            emplace_back(0);
            size--;
        }
    }

    template<class T>
    list<T>::list(const list& rhs) {
        p_init();
        for (auto& val : rhs) {
            emplace_back(val);
        }
    }

    template<class T>
    list<T>::list(list&& rhs) {
        p_init();
        for (auto&& val : rhs) {
            emplace_back(std::move(val));
        }
    }

    template<class T>
    list<T>::~list() {
        clear();
        delete m_end;
    }

    template<class T>
    void list<T>::p_init() {
        m_end = new node();
        m_begin = m_end;
        m_pre_end = m_begin;
        this->m_size = 0;
    }

    template<class T>
    typename list<T>::node* list<T>::p_get_before(const node* n) const {
        node* bak;
        for (auto it = begin(); it != end(); it++) {
            if (it.m_node == n) {
                break;
            } else {
                bak = it.m_node;
            }
        }
        return bak;
    }

    template<class T>
    size_t list<T>::size() const {
        return this->m_size;
    }

    template<class T>
    bool list<T>::empty() const {
        return size() == 0;
    }

    template<class T>
    void list<T>::clear() {
        iterator bak;
        for (auto it = begin(); it != end();) {
            bak = it + 1;
            delete it.m_node;
            it = bak;
        }
        this->m_begin = m_pre_end = m_end;
        this->m_size = 0;
    }

    template<class T>
    typename list<T>::iterator list<T>::begin() const {
        return iterator(m_begin);
    }

    template<class T>
    typename list<T>::iterator list<T>::end() const {
        return iterator(m_end);
    }

    template<class T>
    typename list<T>::iterator list<T>::emplace_after(const iterator& it, const T& val) {
        if (size() == 0) {
            return emplace_back(val);
        } else {
            auto new_node = new node(val);
            auto cur_node = it.m_node;
            auto next_node = cur_node->next;
            cur_node->next = new_node;
            new_node->next = next_node;
            m_size++;
            return iterator(new_node);
        }
    }

    template<class T>
    typename list<T>::iterator list<T>::emplace_back(const T& val) {
        node* n = new node(val);
        n->next = m_end;
        if (m_size == 0) {
            m_begin = n;
        } else {
            m_pre_end->next = n;
        }
        m_pre_end = n;
        m_size++;
        return iterator(n);
    }

    template<class T>
    typename list<T>::iterator list<T>::erase(const iterator& it) {
        auto cur = it.m_node;
        auto next = (it + 1).m_node;
        if (cur == m_begin) {
            if (m_begin == m_pre_end) {
                m_pre_end = next;
            }
            delete cur;
            m_begin = next;
        } else {
            auto prev = p_get_before(cur);
            prev->next = next;
            if (cur == m_pre_end) {
                m_pre_end = prev;
            }
            delete cur;
        }
        m_size--;
        return iterator(next);
    }

} // namespace intermediate