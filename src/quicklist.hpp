#ifndef QUICKLIST_H
#define QUICKLIST_H

#include <stdexcept>
#include <unordered_map>
#include <vector>

// implementation of linklist referenced
// https://blog.csdn.net/sinat_31275315/article/details/108129418
// and
// https://blog.csdn.net/juicewyh/article/details/82845320

template <typename _T, typename _Hash> class QuickList;

template <typename _T> struct Node {
  Node<_T> *m_prev;
  Node<_T> *m_next;
  _T m_data;
  Node(const _T &data = _T())
      : m_prev(nullptr), m_next(nullptr), m_data(data) {}
};

template <typename _T> class __ListIterator {
public:
  using Lnode = Node<_T>;
  using Self = __ListIterator<_T>;

private:
  Lnode *m_cur_node;

public:
  __ListIterator(Lnode *node) : m_cur_node(node) {}
  _T &operator*() { return m_cur_node->m_data; }

  Self &operator++() {
    m_cur_node = m_cur_node->m_next;
    return *this;
  }
  //后置++
  const Self operator++(int) {
    __ListIterator<_T> tmp(*this);
    m_cur_node = m_cur_node->m_next;
    return tmp;
  }
  Self &operator--() {
    m_cur_node = m_cur_node->m_prev;
    return *this;
  }
  const Self operator--(int) {
    Self tmp(*this);
    m_cur_node = m_cur_node->m_prev;
    return tmp;
  }

  bool operator!=(const Self &_rhs) const {
    return m_cur_node != _rhs.m_cur_node;
  }
  bool operator==(const Self &_rhs) const {
    return m_cur_node == _rhs.m_cur_node;
  }
  template <typename _T, typename _Hash> friend class QuickList;
};

template <typename _T, typename _Hash> class QuickList {
public:
  typedef Node<_T> Lnode;
  typedef __ListIterator<_T> iterator;

private:
  Lnode *m_dummy_head;
  Lnode *m_dummy_tail;
  // uint64_t m_size = 0;
  std::unordered_map<_T, Lnode *, _Hash> m_hash;

public:
  QuickList() : m_dummy_head(new Lnode()), m_dummy_tail(new Lnode()) {
    m_dummy_head->m_next = m_dummy_tail;
    m_dummy_tail->m_prev = m_dummy_head;
  }
  QuickList(const QuickList<_T, _Hash> &_rhs)
      : m_dummy_head(new Lnode()), m_dummy_tail(new Lnode()) {
    for (auto x : _rhs) {
      PushBack(x);
      m_hash[x] = (--end()).m_cur_node;
    }
  }
  QuickList<_T, _Hash> &operator=(const QuickList<_T, _Hash> _rhs) {
    if (*this == _rhs) {
      return this;
    }
    Clear();
    for (auto x : _rhs) {
      PushBack(x);
      m_hash[x] = (--end()).m_cur_node;
    }
    return *this;
  }
  ~QuickList() {
    Clear();
    delete m_dummy_head;
    delete m_dummy_tail;
  }
  iterator begin() {
    iterator _it(m_dummy_head->m_next);
    return _it;
  }
  iterator end() {
    iterator _it(m_dummy_tail);
    return _it;
  }
  iterator Erase(iterator it) {
    if (Empty()) {
      throw std::runtime_error("empty while erase!");
    }
    auto p1 = it.m_cur_node;
    auto p0 = p1->m_prev;
    auto p2 = p1->m_next;
    p0->m_next = p2;
    p2->m_prev = p0;
    m_hash.erase(p1->m_data);
    delete p1;
  }
  bool Empty() const { return m_hash.empty(); }
  iterator Insert(iterator position, const _T &value) {
    if (m_hash.find(value) != m_hash.end()) {
      return iterator(m_hash[value]);
      // throw std::runtime_error("data alreasy in quicklist.");
    }
    auto _p = position.m_cur_node->m_prev;
    Lnode *newnode = new Lnode(value);
    if (_p == nullptr) {
      throw std::runtime_error("try to insert out of range!");
    } else {
      _p->m_next = newnode;
      newnode->m_prev = _p;
      newnode->m_next = position.m_cur_node;
      position.m_cur_node->m_prev = newnode;
      m_hash[value] = newnode;
    }
    iterator res_it(newnode);
    return res_it;
  }
  void PushBack(const _T &data) { Insert(end(), data); }
  void PopBack() { Erase(--end()); }
  void PushFront(const _T &data) { Insert(begin(), data); }
  void PopFront() { Erase(begin()); }
  uint64_t Size() const { return m_hash.size(); }
  void Clear() {
    if (Empty()) {
      return;
    }
    for (auto __cur = m_dummy_head->m_next->m_next; __cur != nullptr;
         __cur = __cur->m_next) {
      delete __cur->m_prev;
    }
    m_dummy_head->m_next = m_dummy_tail;
    m_dummy_tail->m_prev = m_dummy_head;
    m_hash.clear();
  }
  const _T &Front() {
    if (!Empty()) {
      return m_dummy_head->m_next->m_data;
    } else {
      throw std::runtime_error("try to get data from empty container!");
    }
  }
  const _T &Back() {
    if (!Empty()) {
      return m_dummy_tail->m_prev->m_data;
    } else {
      throw std::runtime_error("try to get data from empty container!");
    }
  }
  iterator Find(const _T &value) {
    if (m_hash.find(value) == m_hash.end()) {
      return end();
    }
    iterator res_it(m_hash.at(value));
    return res_it;
  }

  // for special use
  void PutAfter(const _T &a, const _T &b) {
    if (m_hash.count(a) == 0) {
      throw std::runtime_error("data A not in quicklist");
    } else if (m_hash.count(b) == 0) {
      throw std::runtime_error("data B not in quicklist");
    } else {
      Erase(Find(a));
      Insert(++Find(b), a);
    }
  }
};

#endif