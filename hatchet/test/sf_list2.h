// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
/************************************************************************
 *
 * Copyright (c) 2007 Baidu.com, Inc. All Rights Reserved
 * $Id: sf_list2.h,v 1.2 2010/06/11 03:51:52 scmpf Exp $
 *
 ************************************************************************/

/**

// Brief: һ�������б�ģ����
 * 
 * @notes
 *	- �� _IM_LIST_NO_DELAY �Ѿ�����Ϊ _SF_LIST_NO_DELAY
 *	- ��� _SF_LIST_NO_DELAY �� define���ڵ�ɾ��ʱ�������ͷ��ڴ棬�ܹ������ڴ�ʹ�ã������ܱ�֤multi-thread��ȫ��
 *
 * ʾ��/���Ի�����ʾ:
 * \include test_list.cpp
 **/


#ifndef __SF_LIST2_H__
#define __SF_LIST2_H__

#include <stddef.h>
#include <assert.h>
#include <iterator>
#include <new>
#include <unused/memory_pool.h>

//! ���ڵ����
#define	MAX_NODE_NUM	102400

namespace afs
{

//! ����ڵ�ȽϺ����ӿ�
//! ����ڵ���������ӿ�
typedef bool list_walk_func_t(void *list_item, void *list_param);
//! ����ڵ�ֻ�����������ӿ�
typedef bool list_walk_func_const_t(const void *list_item,
		void *list_param);
//! ����ڵ���������ӿڣ������ýڵ�ĸ����ӽڵ㣩
typedef bool list_walk_ex_func_t(void **node, void *list_param,
		void **prior, void **next);
//! ����ڵ�ֻ�����������ӿڣ������ýڵ�ĸ����ӽڵ㣩
typedef bool list_walk_ex_func_const_t(const void **node, void *list_param,
		void **prior, void **next);
//! ����ڵ㴦�����ӿڣ������ýڵ�ĸ����ӽڵ㣩
typedef bool list_process_func_t(void *node, void *param, void *prior,
		void *next);

//! define of list setting struct
template <typename T>
struct list_setting_t
{
    //typedef int list_comparison_func_t(const T *list_a, const T *list_b, void *list_param);

    //! compare function
    int (*_compare)(const T *list_a, const T *list_b, void *list_param);

    //! compare param
    void *_param;
};

//extern list_setting_t list_setting_default;

//! ȱʡ�����ͱȽϺ���
int list_comp_int(const void *list, const void *list_b, void *list_param);
//! ȱʡ�����ͱȽϺ���������
int list_comp_int_rev(const void *list_a, const void *list_b,
		void *list_param);
//! ȱʡ���ַ����ȽϺ���
int list_comp_str(const void *list_a, const void *list_b,
		void *list_param);
//! ȱʡ���ַ����ȽϺ���������
int list_comp_str_rev(const void *list_a, const void *list_b,
		void *list_param);

/**
 * class of sorted list template
 */
template < class T > class sorted_list_t
{
	public:
		//! ����ڵ㣬����һ�������ֶκ�һ��MPָ���ֶ�
		struct list_node_t
		{
			T list_data;
			MP0_ID next;
		};

		//! structure for interface compatible with STL's map template
		class iterator:public std::iterator < std::forward_iterator_tag, T >
	{
		public:
			// in gcc 2.96, it seems that value_type can't be inherited from std::iterator
			typedef T value_type;

			iterator()
			{
				pMap = NULL;
				pos = MP_NULL;
				ptr = NULL;
			}

			/**
			 * constructor
			 * @param [in] p pointer to sorted list
			 */
			iterator(void *p)
			{
				pMap = (sorted_list_t *) p;
				if (p != NULL)
				{
					pos = pMap->_head;
					ptr = &(pMap->_get_node(pos)->list_data);
				}
				else
				{
					pos = MP_NULL;
					ptr = NULL;
				}
			}
			
			//! param pEntry is useless currently
			iterator(void *pEntry, void *p)
			{
				pMap = (sorted_list_t *) p;
				if (p != NULL)
				{
					pos = pMap->_head;
					ptr = &(pMap->_get_node(pos)->list_data);
				}
				else
				{
					pos = MP_NULL;
					ptr = NULL;
				}
			}

			//! overloaded operator of ==
			bool operator ==(const iterator & iter)
			{
				return iter.ptr == ptr;
			}
			//! overloaded operator of !=
			bool operator !=(const iterator & iter)
			{
				return iter.ptr != ptr;
			}

			//! prefix: increase and then fetch
			iterator & operator ++()
			{
				if (pos != MP_NULL)
				{
					pos = pMap->_get_node(pos)->next;
				}
				if (pos != MP_NULL)
				{
					ptr = &(pMap->_get_node(pos)->list_data);
				}
				else
				{
					ptr = NULL;
				}

				return *this;
			}

			//! postfix: fetch and then increase
			const iterator operator ++(int)
			{
				iterator tmp = *this;

				if (pos != MP_NULL)
				{
					pos = pMap->_get_node(pos)->next;
				}
				if (pos != MP_NULL)
				{
					ptr = &(pMap->_get_node(pos)->list_data);
				}
				else
				{
					ptr = NULL;
				}

				return tmp;
			}

			//! overloaded operator of * 
			value_type & operator *()const
			{
				return (value_type &) * ptr;
			}

			//! overloaded operator of ->
			value_type *operator ->() const
			{
				return (value_type *) ptr;
			}

		protected:
			MP0_ID pos;
			value_type *ptr;
			sorted_list_t *pMap;
	};

		typedef typename iterator::value_type value_type;

		//! ����ָ������ͷ����iterator
		iterator begin()
		{
			return iterator(this);
		}

		//! ���ر�ʾ����ĩβ��iterator
		iterator end()
		{
			return NULL;
		}

	private:
		MP0_ID _self_mp_id;
		MP0_ID _head;
		MP0_ID _tail;
		size_t _count;
		bool _do_sort;
		list_setting_t<T> *_setting;
		static MemoryPool _mp_list;
		static MemoryPool _mp_node;
		static list_setting_t<T> *_sort_setting;

	private:
		inline static sorted_list_t *_get_list(MP0_ID mp_id)
		{
			return (sorted_list_t *) _mp_list.address(mp_id);
		}

		inline static list_node_t *_get_node(MP0_ID mp_id)
		{
			return (list_node_t *) _mp_node.address(mp_id);
		}

		void _qsort(list_node_t * buf[], int begin_pos, int end_pos);

	public:
		/**
		 * constructor
		 * @param [in] setting compare�����ã������ȽϺ����ͱȽϲ���
		 * @param [in] do_sort �Ƿ��������
		 */
		sorted_list_t(list_setting_t<T> * setting = NULL, bool do_sort = true);

		//! desctructor
		~sorted_list_t();

		/**
		 * ʹ�÷���������һ������ɾ������ʹ��destroy������ʹ��delete
		 * @param [in] setting compare�����ã������ȽϺ����ͱȽϲ���
		 * @param [in] do_sort �Ƿ��������
		 */
		static sorted_list_t *create(list_setting_t<T> * setting =
				NULL, bool do_sort = true);

		//! destory��create������list����
		static void destroy(sorted_list_t * sorted_list);

		//! return mp of list
		inline static MemoryPool *get_mp_list(void)
		{
			return &_mp_list;
		}
		//! return mp of node
		inline static MemoryPool *get_mp_node(void)
		{
			return &_mp_node;
		}

		//! does the recycle
		inline static void mp_recycle(void)
		{
			_mp_list.recycle_delayed();
			_mp_node.recycle_delayed();
		}

		//! return the mp size of list
		inline static size_t mp_list_size(void)
		{
			return _mp_list.alloc_count();
		}

		//! return the mp size of list node
		inline static size_t mp_node_size(void)
		{
			return _mp_node.alloc_count();
		}

		//! return the mp capacity of list
		inline static size_t mp_list_capacity(void)
		{
			return _mp_list.capacity();
		}

		//! return the mp capacity of list node
		inline static size_t mp_node_capacity(void)
		{
			return _mp_node.capacity();
		}

		//! �����Ƿ��������
		inline void set_do_sort(bool do_sort)
		{
			_do_sort = do_sort;
		}

		//! ��listβ�˲���ڵ�item
		T * sorted_list_t < T >::append(const T * item,
				list_process_func_t list_process_func = NULL);
		/**
		 * ��list�в���ڵ�item�����do_sort����������룬���������listͷ��
		 */
		T *insert(const T * item, list_process_func_t list_process_func =
				NULL);
		/**
		 * ��listͷ�˲���ڵ�item
		 */
		T *_insert(const T * item);
		/**
		 * ��list_merge�еĽڵ�鲢��this�У�����node��ȵĲ����ظ����룬�鲢������list_mergeԤ��Ϊ��
		 * @param [in,out] list_merge ���鲢list
		 * @param [in] list_merge�еĽڵ�鲢��this֮ǰ��Ҫ���еĲ���
		 */
		bool insert_merge(sorted_list_t & list_merge,
				list_process_func_t list_process_func = NULL);

		/**
		 * �滻list����item��_compareΪ0�Ľڵ㣬���ýڵ���Ϊitem
		 * @return
		 *	- NULL û���ҵ�item�����ҵ��Ľڵ���item bitwise���
		 *	- !NULL �滻�ɹ������ر��滻�ڵ��ָ��
		 */
		inline T *replace(const T * item)
		{
			T *p = find(item);

			if (p == NULL || p == item)
			{
				return NULL;
			}
			else
			{
				*p = *item;
				return p;
			}
		}

		/**
		 * ɾ���ڵ�item������ɾ��ǰ����list_process_func�Խڵ�item����
		 * @return
		 *	- NULL ʧ��
		 *	- !NULL �ɹ�������item
		 */
		const T *remove(const T * item, list_process_func_t list_process_func =
				NULL);
		/**
		 * ɾ���ڵ�item
		 * @return
		 *	- NULL ʧ��
		 *	- !NULL �ɹ�������item
		 */
		const T *_remove(const T * item);
		//! ɾ��list�����нڵ�
		void removeall(void);
		/**
		 * �ڵ�ǰlist��ɾ��ͬlist_merge����ͬ�Ľڵ�
		 * @param [in] list_merge ��������ͬ�ڵ��list
		 * @param [in] list_process_func ɾ����ǰlist����ͬ�ڵ�ǰ��Ҫ���еĲ���
		 * @return
		 *	- true �ɹ�
		 *	- false ʧ��
		 */
		bool remove_merge(sorted_list_t & list_merge,
				list_process_func_t list_process_func = NULL);

		/**
		 * ɾ���ڵ�item���Ƚ�ʱʹ��compare��param������ɾ��ǰ����list_process_func�Խڵ�item����
		 * @return ������ɾ���ڵ�ĸ���
		 */
                /*               template <typename list_comparison_func_t>
		int traverse_remove(const T * item,
				list_process_func_t list_process_func =
                                    NULL, list_comparison_func_t *compare =
				NULL, void *param = NULL);
                */
		//! ���ҽڵ�item������item��ָ��
		T *find(const T * item) const;

		//! NOTE: unsorted list will return NULL only in _find function
		inline T *_find(const T * item) const
		{
			return NULL;
		}

		//! �Խڵ�a��b���бȽ�
		static int compare_node(const void *node_a, const void *node_b)
		{
			return _sort_setting->
				_compare(&((*((list_node_t **) node_a))->list_data),
						&((*((list_node_t **) node_b))->list_data),
						_sort_setting->_param);
		}

		/**
		 * ����qsort������������ʱ�临�Ӷ�Ϊ2n+nlogn
		 * @return �ɹ�ʱ����true��ʧ��ʱ����false
		 */
		bool sort(void);

             /**
		 *  sort the list using qsort and unique the sorted list , time complexity is 3n+nlogn
		 * @return sucess true��fail false
		 */
             bool sort_unique(void);

		/**
		 * �������м���list
		 * @param [in] pdata �����ص������׵�ַ
		 * @param [in] count �����С
		 * @param [in] sorted �����Ƿ�����
		 * @param [in] list_process_func ����ʱ�Խڵ�Ĵ�����
		 * @return ���سɹ�����Ľڵ����
		 */
		int load_from_array(const T pdata[], int count, bool sorted,
				T * plist[] =
				NULL, list_process_func_t list_process_func =
				NULL);
		/**
		 * �������м���list
		 * @param [in] pdata �����ص��������ָ��
		 * @param [in] count �����С
		 * @param [in] sorted �����Ƿ�����
		 * @param [in] list_process_func ����ʱ�Խڵ�Ĵ�����
		 * @return ���سɹ�����Ľڵ����
		 */
		int load_from_array(const T * pdata[], int count, bool sorted,
				T * plist[] =
				NULL, list_process_func_t list_process_func =
				NULL);

		/**
		 * ɾ����һ���ڵ�
		 * @node [in] ��ǰ�ڵ�
		 * @return �ɹ�ʱ����true
		 */
		inline bool remove_next(list_node_t * node)
		{
			assert(node != NULL);

			list_node_t *n;
			MP0_ID p;

			p = node->next;
			n = _get_node(p);
			node->next = n->next;

			_count--;

#ifdef _SF_LIST_NO_DELAY
			_mp_node.dealloc(p);
#else
			_mp_node.delayed_dealloc(p);
#endif

			return true;
		}

		/**
		 * Warning: The value of _count may be incorrect in multi-thread environment
		 */
		inline size_t count() const
		{
			return _count;
		}

		/**
		 * ����list����ǰlist
		 * @return �ɹ�ʱ����true�����򷵻�false
		 */
		bool clone(sorted_list_t * list);

		//! �������нڵ�
		bool traverse(list_walk_func_t list_walk_func, void *param);
		//! �������нڵ�
		bool traverse(list_walk_func_const_t list_walk_func,
				void *param) const;
		//! �������нڵ�
		bool traverse(list_walk_ex_func_t list_walk_func, void *param);
		//! �������нڵ�
		bool traverse(list_walk_ex_func_const_t list_walk_func,
				void *param) const;
		//! �������нڵ�
		bool traverse(list_process_func_t list_process_func, void *param);

		//! Get node count of common list
		int common_list_count(const sorted_list_t & list_comp) const;

		//! ��list_1��list_2�Ĺ����ڵ���뵱ǰlist��
		bool get_common_list(sorted_list_t < T >
				&list_1, sorted_list_t <T> & list_2,
				void *param = NULL,
				list_walk_func_const_t list_process_func = NULL);
};

template < class T > sorted_list_t < T >::sorted_list_t(list_setting_t<T> * setting, bool do_sort):_self_mp_id(MP_NULL),
	_head(MP_NULL), _tail(MP_NULL), _count(0), _do_sort(do_sort),
	_setting(setting)
{
}

template < class T > sorted_list_t < T >::~sorted_list_t()
{
	removeall();

	if (_self_mp_id != MP_NULL)
	{
#ifdef _SF_LIST_NO_DELAY
		_mp_list.dealloc(_self_mp_id);
#else
		_mp_list.delayed_dealloc(_self_mp_id);
#endif
	}
}

template < class T >
	sorted_list_t < T > *sorted_list_t <
T >::create(list_setting_t<T> * setting, bool do_sort)
{
	sorted_list_t *sorted_list = NULL;
	MP0_ID p = MP_NULL;

	if (setting == NULL)
	{
            return NULL;  //setting = &list_setting_default;
	}

	p = _mp_list.alloc();
	if (p == MP_NULL)
	{
		return NULL;
	}

	sorted_list = new(_get_list(p)) sorted_list_t(setting, do_sort);
	sorted_list->_self_mp_id = p;

	return sorted_list;
}

	template < class T >
void sorted_list_t < T >::destroy(sorted_list_t * sorted_list)
{
	assert(sorted_list != NULL);

	sorted_list->~sorted_list_t();
}

	template < class T >
T * sorted_list_t < T >::append(const T * item,
		list_process_func_t list_process_func)
{
	MP0_ID n;
	list_node_t *node;

	n = _mp_node.alloc();
	if (n == MP_NULL)
	{
		return NULL;
	}
	node = _get_node(n);

	node->next = MP_NULL;
	node->list_data = *item;

	if (list_process_func != NULL)
	{
		list_process_func((void *) node, NULL,
				(void *) (_tail != MP_NULL ? _get_node(_tail) : NULL),
				NULL);
	}

	if (_tail != MP_NULL)
	{
		_get_node(_tail)->next = n;
	}
	else
	{
		_head = n;
	}
	_tail = n;

	_count++;

	return &node->list_data;
}

	template < class T >
T * sorted_list_t < T >::insert(const T * item,
		list_process_func_t list_process_func)
{
	MP0_ID p;
	MP0_ID q;
	MP0_ID n;
	list_node_t *node;

	assert(item != NULL);

	if (!_do_sort)
	{
		return _insert(item);
	}

	for (p = _head, q = MP_NULL; p != MP_NULL;)
	{
		node = _get_node(p);
		int cmp =
			_setting->_compare(item, &node->list_data, _setting->_param);

		if (cmp > 0)
		{
			q = p;
			p = node->next;
		}
		else if (cmp < 0)
		{
			// node not exist
			n = _mp_node.alloc();
			if (n == MP_NULL)
			{
				return NULL;
			}
			node = _get_node(n);

			node->next = p;
			node->list_data = *item;

			if (list_process_func != NULL)
			{
				list_process_func((void *) node, NULL,
						(void *) (q !=
							MP_NULL ? _get_node(q) : NULL),
						(void *) _get_node(p));
			}

			if (q != MP_NULL)
			{
				_get_node(q)->next = n;
			}
			else
			{
				_head = n;
			}

			_count++;
			return &node->list_data;
		}
		else
		{
			// node found
			return &node->list_data;
		}
	}

	// node not exist
	n = _mp_node.alloc();
	if (n == MP_NULL)
	{
		return NULL;
	}
	node = _get_node(n);

	node->next = MP_NULL;
	node->list_data = *item;

	if (list_process_func != NULL)
	{
		list_process_func((void *) node, NULL,
				(void *) (q != MP_NULL ? _get_node(q) : NULL),
				NULL);
	}

	if (q != MP_NULL)
	{
		_get_node(q)->next = n;
	}
	else
	{
		_head = n;
	}

	_count++;

	if (MP_NULL == node->next)
	{
		_tail = n;
	}

	return &node->list_data;
}

template < class T > T * sorted_list_t < T >::_insert(const T * item)
{
	MP0_ID n;
	list_node_t *node;

	assert(item != NULL);

	n = _mp_node.alloc();
	if (n == MP_NULL)
	{
		return NULL;
	}
	node = _get_node(n);

	node->next = _head;
	node->list_data = *item;
	_head = n;
	_count++;

	if (MP_NULL == node->next)
	{
		_tail = n;
	}

	return &node->list_data;
}

template < class T >
bool sorted_list_t < T >::insert_merge(sorted_list_t < T > &list_merge,
		list_process_func_t
		list_process_func)
{
	MP0_ID p, q, r;
	list_node_t *node;
	list_node_t *node_add;
	int cmp;

	for (p = _head, q = MP_NULL, r = list_merge._head;
			p != MP_NULL && r != MP_NULL;)
	{
		node = _get_node(p);
		node_add = _get_node(r);
		cmp =
			_setting->_compare(&node_add->list_data, &node->list_data,
					_setting->_param);

		if (cmp > 0)
		{
			q = p;
			p = node->next;
		}
		else if (cmp == 0)
		{
#ifdef _SF_LIST_NO_DELAY
			list_merge._mp_node.dealloc(r);
#else
			list_merge._mp_node.delayed_dealloc(r);
#endif

			r = node_add->next;
			list_merge._head = r;
			list_merge._count--;
		}
		else
		{
			list_merge._head = node_add->next;
			node_add->next = p;

			// This process should be done before inserting node into target list
			if (list_process_func != NULL)
			{
				if (!list_process_func((void *) node_add, NULL,
							(void *) (q !=
								MP_NULL ? _get_node(q) :
								NULL), (void *) node))
				{
					return false;
				}
			}

			if (q != MP_NULL)
			{
				_get_node(q)->next = r;
			}
			else
			{
				_head = r;
			}

			q = r;
			r = list_merge._head;

			_count++;
			list_merge._count--;
		}
	}

	if (list_merge._head != MP_NULL)
	{
		// This process should be done before inserting node into target list
		if (list_process_func != NULL)
		{
			p = q;
			for (r = list_merge._head; r != MP_NULL;)
			{
				node_add = _get_node(r);

				if (!list_process_func((void *) node_add, NULL,
							(void *) (p !=
								MP_NULL ? _get_node(p) :
								NULL), NULL))
				{
					return false;
				}
				p = r;
				r = node_add->next;
			}
		}

		if (q != MP_NULL)
		{
			_get_node(q)->next = list_merge._head;
		}
		else
		{
			_head = list_merge._head;
		}

		_count += list_merge._count;
		list_merge._count = 0;
		list_merge._head = MP_NULL;
	}

	return true;
}

template < class T >
const T *sorted_list_t < T >::remove(const T * item,
		list_process_func_t
		list_process_func)
{
	MP0_ID p;
	MP0_ID q;
	list_node_t *node;

	assert(item != NULL);

	if (!_do_sort)
	{
		return _remove(item);
	}

	for (p = _head, q = MP_NULL; p != MP_NULL;)
	{
		node = _get_node(p);
		int cmp =
			_setting->_compare(item, &node->list_data, _setting->_param);

		if (cmp > 0)
		{
			q = p;
			p = node->next;
		}
		else if (cmp < 0)
		{
			// node not exist
			return NULL;
		}
		else
		{
			// node found
			if (q != MP_NULL)
			{
				_get_node(q)->next = node->next;
			}
			else
			{
				_head = node->next;
			}

			if (list_process_func != NULL)
			{
				list_process_func((void *) node, NULL,
						(void *) (q !=
							MP_NULL ? _get_node(q) : NULL),
						(void *) _get_node(node->next));
			}

#ifdef _SF_LIST_NO_DELAY
			_mp_node.dealloc(p);
#else
			_mp_node.delayed_dealloc(p);
#endif

			_count--;

			return (item);
		}
	}

	// node not exist
	return NULL;
}

template < class T > const T *sorted_list_t < T >::_remove(const T * item)
{
	MP0_ID p;
	MP0_ID q;
	list_node_t *node;

	assert(item != NULL);
	for (p = _head, q = MP_NULL; p != MP_NULL;)
	{
		node = _get_node(p);
		int cmp =
			_setting->_compare(item, &node->list_data, _setting->_param);

		if (cmp != 0)
		{
			q = p;
			p = node->next;
		}
		else
		{
			// node found
			if (q != MP_NULL)
			{
				_get_node(q)->next = node->next;
			}
			else
			{
				_head = node->next;
			}

#ifdef _SF_LIST_NO_DELAY
			_mp_node.dealloc(p);
#else
			_mp_node.delayed_dealloc(p);
#endif

			_count--;

			return (item);
		}
	}

	// node not exist
	return NULL;
}

template < class T > void sorted_list_t < T >::removeall(void)
{
	MP0_ID p;
	MP0_ID q;

	for (p = _head; p != MP_NULL; p = q)
	{
		q = _get_node(p)->next;

#ifdef _SF_LIST_NO_DELAY
		_mp_node.dealloc(p);
#else
		_mp_node.delayed_dealloc(p);
#endif
	}

	_head = MP_NULL;
	_tail = MP_NULL;
	_count = 0;
}

template < class T >
bool sorted_list_t < T >::remove_merge(sorted_list_t < T > &list_merge,
		list_process_func_t
		list_process_func)
{
	MP0_ID p, q, n, r;
	list_node_t *node;
	list_node_t *node_del;

	for (p = _head, q = MP_NULL, r = list_merge._head;
			p != MP_NULL && r != MP_NULL;)
	{
		node = _get_node(p);
		node_del = _get_node(r);
		int cmp =
			_setting->_compare(&node_del->list_data, &node->list_data,
					_setting->_param);

		if (cmp > 0)
		{
			q = p;
			p = node->next;
		}
		else if (cmp == 0)
		{
			if (q != MP_NULL)
			{
				_get_node(q)->next = node->next;
			}
			else
			{
				_head = node->next;
			}

			n = node->next;

			// This process should be done before inserting node into target list
			if (list_process_func != NULL)
			{
				if (!list_process_func((void *) node, NULL,
							(void *) (q !=
								MP_NULL ? _get_node(q) :
								NULL),
							(void *) (n !=
								MP_NULL ? _get_node(n) :
								NULL)))
				{
					return false;
				}
			}

#ifdef _SF_LIST_NO_DELAY
			_mp_node.dealloc(p);
#else
			_mp_node.delayed_dealloc(p);
#endif

			_count--;

			p = n;
			r = node_del->next;
		}
		else					// cmp < 0
		{
			r = node_del->next;
		}
	}

	return true;
}

/*
template < class T, typename list_comparison_func_t>
int sorted_list_t < T >::traverse_remove(const T * item,
		list_process_func_t list_process_func,
		list_comparison_func_t *compare, void *param)
{
	MP0_ID p;
	MP0_ID q;
	MP0_ID n;
	list_node_t *node;
	int count = 0;

	assert(item != NULL);
	for (p = _head, q = MP_NULL; p != MP_NULL;)
	{
		node = _get_node(p);
		int cmp;

		if (compare == NULL)
		{
			cmp =
				_setting->_compare(item, &node->list_data,
						_setting->_param);
		}
		else
		{
			cmp = compare(item, &node->list_data, param);
		}

		if (cmp != 0)
		{
			q = p;
			p = node->next;
		}
		else
		{
			// node found
			n = node->next;
			if (q != MP_NULL)
			{
				_get_node(q)->next = node->next;
			}
			else
			{
				_head = node->next;
			}

			if (list_process_func != NULL)
			{
				list_process_func((void *) node, NULL,
						(void *) (q !=
							MP_NULL ? _get_node(q) : NULL),
						(void *) _get_node(n));
			}

#ifdef _SF_LIST_NO_DELAY
			_mp_node.dealloc(p);
#else
			_mp_node.delayed_dealloc(p);
#endif

			_count--;
			p = n;

			count++;
		}
	}

	return count;
}
*/

template < class T > T * sorted_list_t < T >::find(const T * item) const
{
	MP0_ID p;
	list_node_t *node;

	assert(item != NULL);

	if (!_do_sort)
	{
		return _find(item);
	}

	for (p = _head; p != MP_NULL;)
	{
		node = _get_node(p);
		int cmp =
			_setting->_compare(item, &node->list_data, _setting->_param);

		if (cmp > 0)
		{
			p = node->next;
		}
		else if (cmp < 0)
		{
			// node not exist
			return NULL;
		}
		else
		{
			// node found
			return &node->list_data;
		}
	}

	// node not exist
	return NULL;
}

template < class T > bool sorted_list_t < T >::sort(void)
{
	static list_node_t **buf = NULL;
	static size_t buf_size = MAX_NODE_NUM;
	MP0_ID p;
	MP0_ID q;
	list_node_t *n;
	unsigned int i;

	if (_do_sort || _count == 0)
	{
		_do_sort = true;
		return true;
	}

	if (buf == NULL)
	{
		buf = (list_node_t **) malloc(sizeof(list_node_t *) * buf_size);
		if (buf == NULL)
		{
			return false;
		}
	}

	if (_count > buf_size)
	{
		buf_size = _count + _count / 5;

		free(buf);

		buf = (list_node_t **) malloc(sizeof(list_node_t *) * buf_size);
		if (buf == NULL)
		{
			return false;
		}
	}

	for (i = 0, p = _head; i < _count && p != MP_NULL; i++)
	{
		n = _get_node(p);
		q = n->next;
		n->next = p;			// Set next to the node itself
		buf[i] = n;
		p = q;
	}

	_sort_setting = _setting;

	// sort list
	qsort(buf, _count, sizeof(list_node_t *), compare_node);
	//_qsort(buf, 0, _count - 1);

	// restore list
	if (_count == 0)
	{
		_head = MP_NULL;
	}
	else
	{
		_head = buf[0]->next;
	}

	for (i = 0; i < _count - 1; i++)
	{
		buf[i]->next = buf[i + 1]->next;
	}
	buf[i]->next = MP_NULL;

	_do_sort = true;

	return true;
}

template < class T > bool sorted_list_t < T >::sort_unique(void)
{
	static list_node_t **buf = NULL;
	static size_t buf_size = MAX_NODE_NUM;
	MP0_ID p;
	MP0_ID q;
	list_node_t *n = NULL;
	
      list_node_t *current_node = NULL;
      list_node_t *next_node = NULL;
      MP0_ID current_id = MP_NULL;
	MP0_ID next_id = MP_NULL;

      unsigned int next_index;
      unsigned int total_count = 0;

	if (_do_sort || _count == 0)
	{
		_do_sort = true;
		return true;
	}

	if (buf == NULL)
	{
		buf = (list_node_t **) malloc(sizeof(list_node_t *) * buf_size);
		if (buf == NULL)
		{
			return false;
		}
	}

	if (_count > buf_size)
	{
		buf_size = _count + _count / 5;

		free(buf);

		buf = (list_node_t **) malloc(sizeof(list_node_t *) * buf_size);
		if (buf == NULL)
		{
			return false;
		}
	}

      	total_count = 0;
	unsigned int i = 0;
	for (i = 0, p = _head; i < _count && p != MP_NULL; i++)
	{
		n = _get_node(p);
		q = n->next;
		n->next = p;			// Set next to the node itself
		buf[i] = n;
		p = q;
             total_count++;
	}

	_sort_setting = _setting;

	// sort list
	qsort(buf, total_count, sizeof(list_node_t *), compare_node);
	//_qsort(buf, 0, _count - 1);

	// restore list
	if (total_count == 0)
	{
		_head = MP_NULL;
            return true;
	}
	
       _head = buf[0]->next;


      int cmp = 0;
      unsigned int current_index = 0;
      
	for (next_index = 1; next_index < total_count; next_index++)
	{
	     current_id = buf[current_index]->next;
             next_id = buf[next_index]->next;
             
	     current_node= _get_node(current_id);
             next_node =  _get_node(next_id);
             
	     cmp = _setting->_compare(&(current_node->list_data), &(next_node->list_data), _setting->_param);

             if (cmp == 0)
             {
                #ifdef _SF_LIST_NO_DELAY
		       _mp_node.dealloc(next_id);
                #else
        		 _mp_node.delayed_dealloc(next_id);
                #endif
             }
             else
             {
		 buf[current_index]->next = buf[next_index]->next;
                 current_index = next_index;
             }
	}
    
	buf[current_index]->next = MP_NULL;

	_do_sort = true;

	return true;
}

	template < class T >
void sorted_list_t < T >::_qsort(list_node_t * buf[], int begin_pos,
		int end_pos)
{
	int i = begin_pos;
	int j = end_pos;
	int cmp;
	list_node_t *node = buf[begin_pos];

	while (i < j)
	{
		while (i < j)
		{
			cmp =
				_setting->_compare(&(buf[j]->list_data), &node->list_data,
						_setting->_param);
			if (cmp < 0)
			{
				buf[i] = buf[j];
				i++;
				break;
			}
			j--;
		}
		while (i < j)
		{
			cmp =
				_setting->_compare(&(buf[i]->list_data), &node->list_data,
						_setting->_param);
			if (cmp > 0)
			{
				buf[j] = buf[i];
				j--;
				break;
			}
			i++;
		}
	}

	buf[i] = node;

	if (begin_pos < i - 1)
	{
		_qsort(buf, begin_pos, i - 1);
	}
	if (j + 1 < end_pos)
	{
		_qsort(buf, j + 1, end_pos);
	}
}

template < class T >
int sorted_list_t < T >::load_from_array(const T pdata[], int count,
		bool sorted, T * plist[],
		list_process_func_t
		list_process_func)
{
	MP0_ID n;
	list_node_t *node;
	T *data;
	int ret = 0;

	assert(pdata != NULL);

	if (count <= 0)
	{
		return 0;
	}

	if (_count == 0 && _head == MP_NULL && sorted)
	{
		while (count > 0)
		{
			n = _mp_node.alloc();
			if (n == MP_NULL)
			{
				return 0;
			}
			node = _get_node(n);

			node->list_data = pdata[--count];
			node->next = _head;
			_head = n;
			_count++;
			if (plist != NULL)
			{
				plist[ret++] = &node->list_data;
			}
		}

		if (list_process_func != NULL)
		{
			traverse(list_process_func, NULL);
		}

		return _count;
	}
	else
	{
		for (int i = 0; i < count; ++i)
		{
			data = insert(&pdata[i], list_process_func);
			if (data != NULL)
			{
				if (plist != NULL)
				{
					plist[ret] = data;
				}
				ret++;
			}
		}

		return ret;
	}

	return 0;
}

template < class T >
int sorted_list_t < T >::load_from_array(const T * pdata[], int count,
		bool sorted, T * plist[],
		list_process_func_t
		list_process_func)
{
	MP0_ID n;
	list_node_t *node;
	T *data;
	int ret = 0;

	assert(pdata != NULL);

	if (count <= 0)
	{
		return 0;
	}

	if (_count == 0 && _head == MP_NULL && sorted)
	{
		while (count > 0)
		{
			n = _mp_node.alloc();
			if (n == MP_NULL)
			{
				return 0;
			}
			node = _get_node(n);

			node->list_data = *pdata[--count];
			node->next = _head;
			_head = n;
			_count++;
			if (plist != NULL)
			{
				plist[ret++] = &node->list_data;
			}
		}

		if (list_process_func != NULL)
		{
			traverse(list_process_func, NULL);
		}

		return _count;
	}
	else
	{
		for (int i = 0; i < count; ++i)
		{
			data = insert(pdata[i], list_process_func);
			if (data != NULL)
			{
				if (plist != NULL)
				{
					plist[ret] = data;
				}
				ret++;
			}
		}

		return ret;
	}

	return 0;
}

	template < class T >
bool sorted_list_t < T >::clone(sorted_list_t < T > *list)
{
	MP0_ID p;
	list_node_t *node;

	if (_count > 0)
	{
		removeall();
	}

	set_do_sort(false);

	for (p = list->_head; p != MP_NULL; p = node->next)
	{
		node = _get_node(p);

		if (!_insert(&(node->list_data)))
		{
			ST_FATAL("invoke _insert failed.");
			return false;
		}
	}

	return true;
}

	template < class T >
bool sorted_list_t < T >::traverse(list_walk_func_t list_walk_func,
		void *param)
{
	MP0_ID p;
	list_node_t *node;

	for (p = _head; p != MP_NULL; p = node->next)
	{
		node = _get_node(p);
		if (!list_walk_func(&node->list_data, param))
		{
			return false;
		}
	}

	return true;
}

template < class T >
bool sorted_list_t <
T >::traverse(list_walk_func_const_t list_walk_func, void *param) const
{
	MP0_ID p;
	list_node_t *node;

	for (p = _head; p != MP_NULL; p = node->next)
	{
		node = _get_node(p);
		if (!list_walk_func(&node->list_data, param))
		{
			return false;
		}
	}

	return true;
}

	template < class T >
bool sorted_list_t < T >::traverse(list_walk_ex_func_t list_walk_func,
		void *param)
{
	MP0_ID p;
	list_node_t *node;
	list_node_t *prior = NULL;
	list_node_t *next;

	for (p = _head; p != MP_NULL;)
	{
		node = _get_node(p);
		p = node->next;
		next = _get_node(p);

		if (!list_walk_func
				((void **) &node, param, (void **) &prior, (void **) &next))
		{
			return false;
		}

		prior = node;
	}

	return true;
}

template < class T >
bool sorted_list_t <
T >::traverse(list_walk_ex_func_const_t list_walk_func,
		void *param) const
{
	MP0_ID p;
	list_node_t *node;
	list_node_t *prior = NULL;
	list_node_t *next;

	for (p = _head; p != MP_NULL;)
	{
		node = _get_node(p);
		p = node->next;
		next = _get_node(p);

		if (!list_walk_func
				((const void **) &node, param, (void **) &prior, (void **) &next))
		{
			return false;
		}

		prior = node;
	}

	return true;
}

template < class T >
	bool sorted_list_t <
T >::traverse(list_process_func_t list_process_func, void *param)
{
	MP0_ID p;
	list_node_t *node;
	list_node_t *prior = NULL;
	list_node_t *next;

	for (p = _head; p != MP_NULL;)
	{
		node = _get_node(p);
		p = node->next;
		next = _get_node(p);

		if (!list_process_func
				((void *) node, param, (void *) prior, (void *) next))
		{
			return false;
		}

		prior = node;
	}

	return true;
}

template < class T >
int sorted_list_t < T >::common_list_count(const sorted_list_t < T >
		&list_comp) const
{
	MP0_ID p, q;
	list_node_t *node;
	list_node_t *node_cmp;
	int cmp;
	int count = 0;

	for (p = _head, q = list_comp._head; p != MP_NULL && q != MP_NULL;)
	{
		node = _get_node(p);
		node_cmp = _get_node(q);
		cmp =
			_setting->_compare(&node_cmp->list_data, &node->list_data,
					_setting->_param);

		if (cmp > 0)
		{
			p = node->next;
		}
		if (cmp == 0)
		{
			count++;
			q = node_cmp->next;
		}
		if (cmp < 0)
		{
			q = node_cmp->next;
		}
	}

	return count;
}



template < class T >
bool sorted_list_t < T >::get_common_list(sorted_list_t < T >
		& list_1, sorted_list_t <T> & list_2,
		void *param,
		list_walk_func_const_t list_process_func)
{
	MP0_ID p, q;
	list_node_t *node;
	list_node_t *node_cmp;
	int cmp;
	int count = 0;
	bool ret = false;
	T * data = NULL;

	for (p = list_1._head, q = list_2._head; p != MP_NULL && q != MP_NULL;)
	{
		node = _get_node(p);
		node_cmp = _get_node(q);
		cmp =
			_setting->_compare(&node_cmp->list_data, &node->list_data,
					_setting->_param);

		if (cmp > 0)
		{
			p = node->next;
		}
		if (cmp == 0)
		{
			count++;
			q = node_cmp->next;
			p = node->next;
			data = append(&(node->list_data));
			if (NULL == data)
			{
				ST_WARN("fail to insert node to list");
				continue;
			}
			if (NULL != list_process_func)
			{
				ret = list_process_func((void *) &node->list_data, param);
				if(false == ret)
				{
					ST_WARN("fail to list_process_func");
					continue;
				}
			}

		}
		if (cmp < 0)
		{
			q = node_cmp->next;
		}
	}

	if(count > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

}

#endif  //__SF_LIST_H__

