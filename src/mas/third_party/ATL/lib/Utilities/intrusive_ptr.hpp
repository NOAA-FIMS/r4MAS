#ifndef INTRUSIVE_PTR_HPP_
#define INTRUSIVE_PTR_HPP_

namespace atl {

template<class T> class intrusive_ptr {
	T *ptr_m;
public:

	typedef T element_type;

	constexpr intrusive_ptr() :
			ptr_m(0) {
	}
	intrusive_ptr(T *p, bool add_ref = true) :
			ptr_m(p) {
		if (ptr_m != 0 && add_ref)
			intrusive_ptr_add_ref(ptr_m);
	}

	intrusive_ptr(intrusive_ptr const &r) :
			ptr_m(r.get()) {
		if (ptr_m != 0)
			intrusive_ptr_add_ref(ptr_m);
	}

	~intrusive_ptr() {
		if (ptr_m != 0)
			this->intrusive_ptr_release(ptr_m);
	}

	intrusive_ptr& operator=(intrusive_ptr const &r) {
		ptr_m = r.get();
		intrusive_ptr_add_ref(ptr_m);
		return *this;
	}

	intrusive_ptr& operator=(T *r) {
		ptr_m = r;
		intrusive_ptr_add_ref(ptr_m);
		return *this;
	}

	void reset() {
		this->intrusive_ptr_release(ptr_m);
		this->ptr_m = 0;
	}

	T& operator*() const {
		return *ptr_m;
	}
	// never throws
	T* operator->() const {
		return ptr_m;
	}

	T* get() const {
		return ptr_m;
	}

	void swap(intrusive_ptr &b) {
		T *tmp = ptr_m;
		ptr_m = b.ptr_m;
		b.ptr_m = tmp;
	}
private:

	void intrusive_ptr_add_ref(T *px) {
		++(px->references);
	}

	void intrusive_ptr_release(T *px) {
		px->references--;
		if (px->references == 0) {
			delete px;
		}
	}
};
template<class T, class U> inline bool operator==(
		atl::intrusive_ptr<T> const &a, atl::intrusive_ptr<U> const &b) {
	return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(
		atl::intrusive_ptr<T> const &a, atl::intrusive_ptr<U> const &b) {
	return a.get() != b.get();
}

template<class T, class U> inline bool operator==(
		atl::intrusive_ptr<T> const &a, U *b) {
	return a.get() == b;
}

template<class T, class U> inline bool operator!=(
		atl::intrusive_ptr<T> const &a, U *b) {
	return a.get() != b;
}

template<class T, class U> inline bool operator==(T *a,
		atl::intrusive_ptr<U> const &b) {
	return a == b.get();
}

template<class T, class U> inline bool operator!=(T *a,
		atl::intrusive_ptr<U> const &b) {
	return a != b.get();
}

template<class T> inline bool operator <(atl::intrusive_ptr<T> const &a,
		atl::intrusive_ptr<T> const &b) {
	return std::less<T*>()(a.get(), b.get());
}
template<class T> struct hash;

template<class T> std::size_t hash_value(atl::intrusive_ptr<T> const &p) {
	return std::hash<T*>()(p.get());
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


#endif /* INTRUSIVE_PTR_HPP_ */
