#ifndef INTRUSIVE_PTR_H
#define INTRUSIVE_PTR_H

#include  <cstdint>

namespace atl {

template<typename T>
inline void intrusive_ptr_add_ref(T *p) {
	p->references++;
}

template<typename T>
inline void intrusive_ptr_release(T *p) {
	p->references--;
	if (p->references == 0) {
		delete p;
	}
}

template<typename T>
class intrusive_ptr {
protected:

	template<typename U> friend class intrusive_ptr;
	typedef intrusive_ptr<T> this_type;

	T *ptr_m;

public:

	typedef T element_type;

	intrusive_ptr() :
			ptr_m(NULL) {
	}

	intrusive_ptr(T *p, bool add_ref = true) :
			ptr_m(p) {
		if (ptr_m && add_ref)
			intrusive_ptr_add_ref(ptr_m);
	}

	intrusive_ptr(const intrusive_ptr &other) :
			ptr_m(other.ptr_m) {
		if (ptr_m)
			intrusive_ptr_add_ref(ptr_m);
	}

	intrusive_ptr(intrusive_ptr &&other) :
			ptr_m(nullptr) {
		swap(other);
	}

	template<typename U>
	intrusive_ptr(const intrusive_ptr<U> &other) :
			ptr_m(other.ptr_m) {
		if (ptr_m)
			intrusive_ptr_add_ref(ptr_m);
	}

	~intrusive_ptr() {
		if (ptr_m)
			intrusive_ptr_release(ptr_m);
	}

	intrusive_ptr& operator=(const intrusive_ptr &other) {
		return operator=(other.ptr_m);
	}

	intrusive_ptr& operator=(intrusive_ptr &&other) {
		swap(other);
		return *this;
	}

	template<typename U>
	intrusive_ptr& operator=(const intrusive_ptr<U> &other) {
		return operator=(other.ptr_m);
	}

	intrusive_ptr& operator=(T *pointer) {
		if (pointer != ptr_m) {
			T *const temp = ptr_m; // Create temporary to prevent possible problems with re-entrancy.
			if (pointer)
				intrusive_ptr_add_ref(pointer);
			ptr_m = pointer;
			if (pTemp)
				intrusive_ptr_release(temp);
		}
		return *this;
	}

	T& operator *() const {
		return *ptr_m;
	}

	T* operator ->() const {
		return ptr_m;
	}

	T* get() const {
		return ptr_m;
	}

	void reset() {
		T *const temp = ptr_m;
		ptr_m = NULL;
		if (temp)
			intrusive_ptr_release(temp);
	}

	void swap(this_type &other) {
		T *const temp = ptr_m;
		ptr_m = other.ptr_m;
		other.ptr_m = temp;
	}

	void attach(T *pointer) {
		T *const temp = ptr_m;
		ptr_m = pointer;
		if (temp)
			intrusive_ptr_release(temp);
	}

	T* detach() {
		T *const temp = ptr_m;
		ptr_m = NULL;
		return temp;
	}

	typedef T* (this_type::*bool_)() const;
	operator bool_() const {
		if (ptr_m)
			return &this_type::get;
		return NULL;
	}

	bool operator!() const {
		return (ptr_m == NULL);
	}

};

template<typename T>
inline T* get_pointer(const intrusive_ptr<T> &pointer) {
	return pointer.get();
}

template<typename T>
inline void swap(intrusive_ptr<T> &pointer_1, intrusive_ptr<T> &pointer_2) {
	pointer_1.swap(pointer_2);
}

template<typename T, typename U>
bool operator==(intrusive_ptr<T> const &pointer_1,
		intrusive_ptr<U> const &pointer_2) {
	return (pointer_1.get() == pointer_2.get());
}

template<typename T, typename U>
bool operator!=(intrusive_ptr<T> const &pointer_1,
		intrusive_ptr<U> const &pointer_2) {
	return (pointer_1.get() != pointer_2.get());
}

template<typename T>
bool operator==(intrusive_ptr<T> const &pointer_1, T *p) {
	return (pointer_1.get() == p);
}

template<typename T>
bool operator!=(intrusive_ptr<T> const &pointer_1, T *p) {
	return (pointer_1.get() != p);
}

template<typename T>
bool operator==(T *p, intrusive_ptr<T> const &pointer_2) {
	return (p == pointer_2.get());
}

template<typename T>
bool operator!=(T *p, intrusive_ptr<T> const &pointer_2) {
	return (p != pointer_2.get());
}

template<typename T, typename U>
bool operator<(intrusive_ptr<T> const &pointer_1,
		intrusive_ptr<U> const &pointer_2) {
	return ((uintptr_t) pointer_1.get() < (uintptr_t) pointer_2.get());
}

template<class T, class U>
intrusive_ptr<T> static_pointer_cast(const intrusive_ptr<U> &intrusivePtr) {
	return static_cast<T*>(intrusivePtr.get());
}

}


namespace std {
template<class T>
struct hash<atl::intrusive_ptr<T> > {
	size_t operator()(const atl::intrusive_ptr<T>  &k) const {
		// Compute individual hash values for first, second and third
		// http://stackoverflow.com/a/1646913/126995
		size_t res = reinterpret_cast<size_t>(k.get());
		return res;
	}
};
}
#endif

