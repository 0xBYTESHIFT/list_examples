#pragma once

namespace pro {

    template<class T, class Alloc = std::allocator<T>>
    class list {
        struct node {
            T* val = nullptr;
            node* next = nullptr;
            Alloc* alloc;

            node(const node& n) {
                *(this) = n;
            }

            node(Alloc* alloc) {
                this->alloc = alloc;
            }

            template<class Arg>
            node(Alloc* alloc, Arg&& val)
                : alloc(alloc) {
                this->val = alloc->allocate(1);
                *(this->val) = std::forward<Arg>(val);
            }

            ~node() {
                if (val)
                    alloc->deallocate(val, 1);
            }

            node& operator=(const node& n) {
                this->alloc = n.alloc;
                this->val = n.val;
                this->next = n.next;
                return *this;
            }
        };
        node *m_begin, *m_pre_end, *m_end;
        size_t m_size;
        Alloc alloc;

        void p_init();
        node* p_get_before(const node* n) const;

        class iterator_base {
        protected:
            friend class pro::list<T, Alloc>;
            node* m_node;
            iterator_base(node* n);

        public:
            iterator_base() = default;
            iterator_base(const iterator_base& it);
            iterator_base(iterator_base&& it);
            ~iterator_base() = default;

            auto operator*() const -> const T&;
            auto operator==(const iterator_base& it) const -> bool;
            auto operator!=(const iterator_base& it) const -> bool = default;
        };

    public:
        class iterator final : public iterator_base {
            class const_iterator;
            friend class pro::list<T, Alloc>;

            iterator(node* n);

        public:
            using iterator_category = std::forward_iterator_tag;

            iterator() = default;

            auto operator*() -> T&;
            auto operator=(const iterator& rhs) -> iterator&;
            auto operator++() -> iterator&;
            auto operator++(int) -> iterator;
            auto operator+(size_t i) const -> iterator;
        };

        class const_iterator final : public iterator_base {
            friend class pro::list<T, Alloc>;
            const_iterator(node* n);

        public:
            using iterator_category = std::forward_iterator_tag;

            const_iterator() = default;

            auto operator=(const iterator_base& rhs) -> const_iterator&;
            auto operator++() -> const_iterator&;
            auto operator++(int) -> const_iterator;
            auto operator+(size_t i) const -> const_iterator;
        };

        using value_type = typename Alloc::value_type;
        using size_type = typename Alloc::size_type;
        using difference_type = typename Alloc::difference_type;

        list(size_t size = 0);
        list(const list& rhs);
        list(list&& rhs);

        ~list();

        auto size() const -> size_t;
        auto empty() const -> bool;
        auto clear() -> void;

        auto begin() -> iterator;
        auto end() -> iterator;
        auto begin() const -> const_iterator;
        auto end() const -> const_iterator;
        auto cbegin() const -> const_iterator;
        auto cend() const -> const_iterator;

        template<class It, class Arg>
        auto emplace_after(It it, Arg&& val) -> iterator;

        template<class Arg>
        auto emplace_back(Arg&& val) -> iterator;

        template<class It>
        auto erase(It it) -> iterator;
    };

    //iterator_base
    template<class T, class Alloc>
    list<T, Alloc>::iterator_base::iterator_base(node* n) {
        this->m_node = n;
    }

    template<class T, class Alloc>
    list<T, Alloc>::iterator_base::iterator_base(const iterator_base& it) {
        this->m_node = it.m_node;
    }

    template<class T, class Alloc>
    list<T, Alloc>::iterator_base::iterator_base(iterator_base&& it) {
        this->m_node = std::move(it.m_node);
        it.m_node = nullptr;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::iterator_base::operator*() const -> const T& {
        return *this->m_node->val;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::iterator_base::operator==(const iterator_base& it) const -> bool {
        return this->m_node == it.m_node;
    }

    //iterator
    template<class T, class Alloc>
    list<T, Alloc>::iterator::iterator(node* n)
        : iterator_base(n) {}

    template<class T, class Alloc>
    auto list<T, Alloc>::iterator::operator*() -> T& {
        return *this->m_node->val;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::iterator::operator=(const iterator& rhs) -> iterator& {
        this->m_node = rhs.m_node;
        return *this;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::iterator::operator++() -> iterator& {
        this->m_node = this->m_node->next;
        return *this;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::iterator::operator++(int) -> iterator {
        auto copy = *this;
        ++(*this);
        return copy;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::iterator::operator+(size_t i) const -> iterator {
        iterator bak(*this);
        while (i-- > 0) {
            bak++;
        }
        return bak;
    }

    //const_iterator
    template<class T, class Alloc>
    list<T, Alloc>::const_iterator::const_iterator(node* n)
        : iterator_base(n) {}

    template<class T, class Alloc>
    auto list<T, Alloc>::const_iterator::operator=(const iterator_base& rhs) -> const_iterator& {
        this->m_node = rhs.m_node;
        return *this;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::const_iterator::operator++() -> const_iterator& {
        this->m_node = this->m_node->next;
        return *this;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::const_iterator::operator++(int) -> const_iterator {
        auto copy = *this;
        ++(*this);
        return copy;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::const_iterator::operator+(size_t i) const -> const_iterator {
        iterator bak(*this);
        while (i-- > 0) {
            bak++;
        }
        return bak;
    }

    //list
    template<class T, class Alloc>
    list<T, Alloc>::list(size_t size) {
        p_init();
        this->m_size = size;
        auto bak = begin();
        while (size > 0) {
            emplace_back(0);
            size--;
        }
    }

    template<class T, class Alloc>
    list<T, Alloc>::list(const list& rhs)
        : list(rhs.size()) {
        std::copy(rhs.begin(), rhs.end(), this->begin());
    }

    template<class T, class Alloc>
    list<T, Alloc>::list(list&& rhs) {
        std::move(rhs.begin(), rhs.end(), std::back_inserter(*this));
    }

    template<class T, class Alloc>
    list<T, Alloc>::~list() {
        clear();
        delete m_end;
    }

    template<class T, class Alloc>
    void list<T, Alloc>::p_init() {
        m_end = new node(&alloc);
        m_begin = m_end;
        m_pre_end = m_begin;
        this->m_size = 0;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::p_get_before(const node* n) const -> node* {
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

    template<class T, class Alloc>
    auto list<T, Alloc>::size() const -> size_t {
        return this->m_size;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::empty() const -> bool {
        return size() == 0;
    }

    template<class T, class Alloc>
    void list<T, Alloc>::clear() {
        iterator bak;
        for (auto it = begin(); it != end();) {
            bak = it + 1;
            delete it.m_node;
            it = bak;
        }
        this->m_begin = m_pre_end = m_end;
        this->m_size = 0;
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::begin() -> iterator {
        return iterator(const_cast<node*>(m_begin));
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::end() -> iterator {
        return iterator(m_end);
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::begin() const -> const_iterator {
        return const_iterator(m_begin);
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::end() const -> const_iterator {
        return const_iterator(m_end);
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::cbegin() const -> const_iterator {
        return const_iterator(m_begin);
    }

    template<class T, class Alloc>
    auto list<T, Alloc>::cend() const -> const_iterator {
        return const_iterator(m_end);
    }

    template<class T, class Alloc>
    template<class It, class Arg>
    auto list<T, Alloc>::emplace_after(It it, Arg&& val) -> iterator {
        if (size() == 0) {
            return emplace_back(val);
        } else {
            auto new_node = new node(&alloc, std::forward<Arg>(val));
            auto cur_node = it.m_node;
            auto next_node = cur_node->next;
            cur_node->next = new_node;
            new_node->next = next_node;
            m_size++;
            return iterator(new_node);
        }
    }

    template<class T, class Alloc>
    template<class Arg>
    auto list<T, Alloc>::emplace_back(Arg&& val) -> iterator {
        node* n = new node(&alloc, std::forward<Arg>(val));
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

    template<class T, class Alloc>
    template<class It>
    auto list<T, Alloc>::erase(It it) -> iterator {
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

} // namespace pro