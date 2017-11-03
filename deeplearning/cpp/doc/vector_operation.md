# vector操作详解

1.vector是线性容器，它的元素严格的按照线性序列排序，可以看成是动态数组。和数组一样,它的元素存储在一块连续的存储空间中，这也意味着我们不仅可以使用迭代器(iterator)访问元素，还可以使用指针的偏移方式访问。
和常规数组不一样的是，vector能够自动存储元素，可以自动增长或缩小存储空间。以下是vector的优点:
(1). 可以使用下标访问个别的元素
(2). 迭代器可以按照不同的方式遍历容器
(3). 可以在容器的末尾增加或删除元素
和数组相比，虽然容器在自动处理容量的大小时会消耗更多的内存，但是容器能提供和数组一样的性能，而且能很好的调整存储空间大小。和其他标准的顺序容器相比(deques or lists)，能更有效访问容器内的元素和在末尾添加和删除元素，在其他位置添加和删除元素，vector则不及其他顺序容器，在迭代器和引用也不比lists支持的好。

容器的大小和容器的容量是有区别的，大小是指元素的个数。容量是分配的内存大小，容量一般等于或大于容器的大小。
vector::size()返回容器的大小，vector::capacity()返回容量值，容量多于容器大小的部分用于以防容器的增加使用，每次重新分配内存都会很影响程序的性能，所以一般分配的容量大于容器的大小。
若要自己指定分配的容量的大小，则可以使用vector::reserve()，但是规定的值要大于size()值。
 
2. 基本操作
(1)头文件#include<vector>.
(2)创建vector对象，vector<int> vec;
(3)尾部插入数字：vec.push_back(a);
(4)使用下标访问元素，cout<<vec[0]<<endl;记住下标是从0开始的。
(5)使用迭代器访问元素.
```cpp
vector<int>::iterator it;  
for(it=vec.begin();it!=vec.end();it++)  
    cout<<*it<<endl;
```
(6)插入元素：    vec.insert(vec.begin()+i,a); 在第i个元素后面插入a;
(7)删除元素：    
```cpp
vec.erase(vec.begin()+2);                        |删除第3个元素
vec.erase(vec.begin()+i,vec.end()+j);       |删除区间[i,j-1];区间从0开始
```
(8)向量大小:vec.size();
(9)清空:vec.clear()　　　|清空之后，vec.size()为０

以下是vector的所有成员函数：
|vector方法(常用)|功能|
|:--------------|:-----------|
|c.begin()                  | 传回迭代器中的一个数据。|
|c.end()                   | 指向迭代器中的最后一个数据地址。|
|c.clear()                  | 移除容器中所有数据。|
|c.empty()                | 判断容器是否为空。|
|c.erase(pos)           | 删除pos位置的数据，传回下一个数据的位置。|
|c.erase(beg,end)    | 删除[beg,end)区间的数据，传回下一个数据的位置。|
|c.capacity()             | 返回容器中数据个数。|
|get_allocator         | 使用构造函数返回一个拷贝。|
|c.push_back(elem)   | 在尾部加入一个数据。|
|c.insert(pos,elem)  | 在pos位置插入一个elem拷贝，传回新数据位置。|
|c.insert(pos,n,elem) | 在pos位置插入n个elem数据。无返回值。|
|c.insert(pos,beg,end) | 在pos位置插入在[beg,end)区间的数据。无返回值。|
|c.max_size()            | 返回容器中最大数据的数量。|
|c.pop_back()            | 删除最后一个数据。|
|c.size()             | 返回容器中实际数据的个数。|
|c1.swap(c2)     | 将c1和c2元素互换。|
|vector c1(c2) | 用c2拷贝c1|
|vector c(n)  | 创建一个vector，含有n个数据，数据均已缺省构造产生。|
|vector c(n, elem) | 创建一个含有n个elem拷贝的vector。|
|vector c(beg,end) |  创建一个以[beg;end)区间的vector。|

|vector方法|功能|
|:--------------|:-----------|
|c.~vector () |  销毁所有数据，释放内存。|
|operator[] | 返回容器中指定位置的一个引用。|
|swap(c1,c2)     | 同上操作。|
|c.rbegin()          | 传回一个逆向队列的第一个数据。|
|c.rend()            | 传回一个逆向队列的最后一个数据的下一个位置。|
|c.resize(num)    | 重新指定队列的长度。|
|c.reserve()        | 保留适当的容量。|
|c.front()                 | 传回第一个数据。|
|c.assign(beg,end)     | 将[beg; end)区间中的数据赋值给c。|
|c.assign(n,elem)       | 将n个elem的拷贝赋值给c。|
|c.at(idx)                   | 传回索引idx所指的数据，如果idx越界，抛出out_of_range。|
|c.back()                   | 传回最后一个数据，不检查这个数据是否存在。|

(1). 构造和复制构造函数
```cpp
explicit vector ( const Allocator& = Allocator() );
explicit vector ( size_type n, const T& value= T(), const Allocator& = Allocator() );
template <class InputIterator>
vector ( InputIterator first, InputIterator last, const Allocator& = Allocator() );
vector ( const vector<T,Allocator>& x );

explicit:是防止隐式转换, Allocator是一种内存分配模式,一般是使用默认的
```cpp
vector<int> A;  //创建一个空的的容器
vector<int> B(10,100); //创建一个个元素,每个元素值为100
vector<int> C(B.begin(),B.end()); //使用迭代器,可以取部分元素创建一个新的容器
vector<int> D(C); //复制构造函数,创建一个完全一样的容器
```

(2). 析构函数
~vector()
销毁容器对象并回收了所有分配的内存
 
(3). 重载了`=`符号
vector<int> E;
E = B; //使用=符号
B = vector<int>(); //将B置为空容器
 
 
(4). vector::begin()  返回第一个元素的迭代器
  函数原型：
  iterator begin ();  //返回一个可变迭代器
const_iterator begin () const; //返回一个常量的迭代器，不可变
 
(5).vector::end()  返回的是越界后的第一个位置，也就是最后一个元素的下一个位置
 
(6).vector::rbegin() 反序的第一个元素，也就是正序最后一个元素
(7).vector::rend() 反序的最后一个元素下一个位置，也相当于正序的第一个元素前一个位置
(8).vector::size() 返回容器中元素个数
(9).vector::max_size()
  size_type max_size () const;
  返回容器的最大可以存储的元素个数，这是个极限，当容器扩展到这个最大值时就不能再自动增大
(10). vector::resize()
  void resize ( size_type sz, T c = T() );
  重新分配容器的元素个数，这个还可以改容器的容量，如果重新分配的元素个数比原来的小，将截断序列，后面的部分丢弃，如果大于原来的个数，后面的值是c的值，默认为0
(11). vector::capacity()
   size_type capacity () const;
   返回vector的实际存储空间的大小，这个一般大于或等于vector元素个数，注意与size()函数的区别
(12). vector::empty()
   bool empty () const;
   当元素个数为0时返回true，否则为false，根据的是元素个数而不是容器的存储空间的大小
(13). vector::reserve()
   void reserve ( size_type n );
重新分配空间的大小，不过这个n值要比原来的capacity()返回的值大，不然存储空间保持不变，n值要比原来的实际存储空间大才能重新分配空间，但是最大值不可以大于max_size的值，否则会抛出异常
(14). vector::operator[]  //重载了[]符号
   reference  operator[] ( size_type n );
const_reference  operator[] ( size_type n ) const;
实现了下标访问元素
(15). vector::at()
   const_reference at ( size_type n ) const;
   reference at ( size_type n );
   在函数的操作方面和下标访问元素一样，不同的是当这个函数越界时会抛出一个异常out_of_range
(16). vector::front()
   reference front ( );
const_reference front ( ) const;
返回第一个元素的值，与begin()函数有区别，begin()函数返回的是第一个元素的迭代器
(17). vector::back()
   reference back ( );
const_reference back ( ) const;
同样，返回最后一个元素的值，注意与end()函数的区别
(18). vector::assign()
   template <class InputIterator> void assign ( InputIterator first, InputIterator last );
void assign ( size_type n, const T& u );
将丢弃原来的元素然后重新分配元素，第一个函数是使用迭代器，第二个函数是使用n个元素，每个元素的值为u。
(19). vector::push_back()
   void push_back ( const T& x );
   在容器的最后一个位置插入元素x,如果size值大于capacity值，则将重新分配空间
(20). vector::pop_back()
   void pop_back ( );
   删除最后一个元素
(21). vector::insert()
   iterator insert ( iterator position, const T& x );
   void insert ( iterator position, size_type n, const T& x );
template <class InputIterator>
void insert ( iterator position, InputIterator first, InputIterator last );
   插入新的元素，
第一个函数，在迭代器指定的位置前插入值为x的元素
第二个函数，在迭代器指定的位置前插入n个值为x的元素
第三个函数，在迭代器指定的位置前插入另外一个容器的一段序列迭代器first到last
若插入新的元素后总得元素个数大于capacity，则重新分配空间
(22). vector::erase()
   iterator erase ( iterator position );
iterator erase ( iterator first, iterator last );
删除元素或一段序列
(23). vector::swap()
   void swap ( vector<T,Allocator>& vec );
   交换这两个容器的内容，这涉及到存储空间的重新分配
(24). vector::clear()
   void clear ( );
   将容器里的内容清空，size值为0，但是存储空间没有改变

3. 算法

(1) 使用`reverse`将元素翻转：需要头文件`#include <algorithm>`;
  reverse(vec.begin(),vec.end())，将元素翻转。在vector中，如果一个函数中需要两个迭代器，一般后一个都不包含。
(2) 使用`sort`排序：需要头文件`#include <algorithm>`;
  sort(vec.begin(),vec.end())；默认是按升序排列，即从小到大。可以通过重写排序比较函数按照降序比较，如下：
 定义排序比较函数：
```cpp
bool Comp(const int &a,const int &b)
{
    return a>b;
}
```
调用时:sort(vec.begin(),vec.end(),Comp)，这样就降序排序。

4. vector容器的内存自增长 
 与其他容器不同，其内存空间只会增长，不会减小。先来看看"C++ Primer"中怎么说：为了支持快速的随机访问，vector容器的元素以连续方式存放，每一个元素都紧挨着前一个元素存储。设想一下，当vector添加一个元素时，为了满足连续存放这个特性，都需要重新分配空间、拷贝元素、撤销旧空间，这样性能难以接受。因此STL实现者在对vector进行内存分配时，其实际分配的容量要比当前所需的空间多一些。就是说，vector容器预留了一些额外的存储区，用于存放新添加的元素，这样就不必为每个新元素重新分配整个容器的内存空间。
 关于vector的内存空间，有两个函数需要注意：`size()`成员指当前拥有的元素个数；`capacity()`成员指当前(容器必须分配新存储空间之前)可以存储的元素个数，一般是2的幂次方。`reserve()`成员可以用来控制容器的预留空间。
 vector另外一个特性在于它的内存空间会自增长，每当vector容器不得不分配新的存储空间时，会以加倍当前容量的分配策略实现重新分配。例如，当前capacity为50，当添加第51个元素时，预留空间不够用了，vector容器会重新分配大小为100的内存空间，作为新连续存储的位置。

5. vector内存释放
 由于vector的内存占用空间只增不减，比如你首先分配了10,000个字节，然后erase掉后面9,999个，留下一个有效元素，但是内存占用仍为10,000个。所有内存空间是在vector析构时候才能被系统回收。empty()用来检测容器是否为空的，clear()可以清空所有元素。但是即使clear()，vector所占用的内存空间依然如故，无法保证内存的回收。
 
 如果需要空间动态缩小，可以考虑使用`deque`。如果非`vector`不可，可以用`swap()`来帮助你释放内存。swap()是交换函数，使vector离开其自身的作用域，从而强制释放vector所占的内存空间，总而言之，释放vector内存最简单的方法是vector<int>.swap(nums)
 当时如果nums是一个类的成员，不能把vector<int>.swap(nums)写进类的析构函数中，否则会导致double free or corruption (fasttop)错误，原因可能是重复释放内存。

5. 利用vector释放指针
 如果vector中存放的是指针，那么当vector销毁时，这些指针指向的对象不会被销毁，那么内存就不会被释放，从而引起大面积的内存泄露。这时应该在销毁`vector`之前将这些指针指向的空间释放(delete)掉。
