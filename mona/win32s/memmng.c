#include	"compiler.h"

struct _memhdr;

typedef struct _memhdr	_MEMHDR;
typedef struct _memhdr	*MEMHDR;

struct _memhdr {
	UINT	size;
	UINT	lock;
	MEMHDR	back;
	MEMHDR	next;
};

static	MEMHDR		used;
static	MEMHDR		unused;
#if defined(_MT)
static	int			csec;
#endif

void memmng_initialize(UINT32 addr, UINT size) {

	used = NULL;
	unused = (MEMHDR)addr;
	unused->size = size;
	unused->lock = 0;
	unused->back = NULL;
	unused->next = NULL;
#if defined(_MT)
	csec = syscall_mutex_create();
#endif
}

void memmng_deinitialize(void) {

#if defined(_MT)
	syscall_mutex_destroy(csec);
#endif
}

static void dump(MEMHDR ptr) {

	while(ptr) {
		TRACEOUT(("%.8x -> %.8x:%.8x %.8x",
				(UINT32)ptr,
				(UINT32)ptr->back, (UINT32)ptr->next, ptr->size));
		ptr = ptr->next;
	}
}

static BOOL linkcheck(MEMHDR ptr) {

	MEMHDR		p;

	while(ptr) {
		p = ptr->back;
		if ((p) && (p->next != ptr)) {
			return(FAILURE);
		}
		p = ptr->next;
		if ((p) && (p->back != ptr)) {
			return(FAILURE);
		}
		ptr = ptr->next;
	}
	return(SUCCESS);
}

static BOOL checkorder(MEMHDR ptr) {

	if (ptr == NULL) {
		return(SUCCESS);
	}
	if (ptr->back != NULL) {
		return(FAILURE);
	}
	while(ptr->next) {
		if ((UINT32)ptr->back >= (UINT32)ptr->next) {
			return(FAILURE);
		}
		ptr = ptr->next;
	}
	return(SUCCESS);
}

static void check(const char *proc) {

	if (linkcheck(used) != SUCCESS) {
		TRACEOUT(("%s: used link failure", proc));
		dump(used);
		exit(1);
	}
	if (linkcheck(unused) != SUCCESS) {
		TRACEOUT(("%s: unused link failure", proc));
		dump(unused);
		exit(1);
	}
	if (checkorder(unused) != SUCCESS) {
		TRACEOUT(("--- %s: unused order failure", proc));
		TRACEOUT(("---- used"));
		dump(used);
		TRACEOUT(("---- unused"));
		dump(unused);
		exit(1);
	}
}

void *memmng_alloc(UINT size) {

	MEMHDR	ptr;
	size_t	remain;
	MEMHDR	fwd;
	MEMHDR	p;

#if defined(_MT)
	syscall_mutex_lock(csec);
#endif
	size = sizeof(_MEMHDR) + ((size + 15) & (~15));
	ptr = unused;
	while(1) {
		if (ptr == NULL) {
#if defined(_MT)
			syscall_mutex_unlock(csec);
#endif
			TRACEOUT(("!!!!! alloc error: %.x", size));
			TRACEOUT(("--- used"));
			dump(used);
			TRACEOUT(("--- unused"));
			dump(unused);
			exit(1);
			return(NULL);
		}
		if (ptr->size >= size) {
			break;
		}
		ptr = ptr->next;
	}
	remain = ptr->size - size;
	if (remain > sizeof(_MEMHDR)) {
		fwd = (MEMHDR)((UINT32)ptr + size);
		fwd->size = remain;
		fwd->lock = 0;
		fwd->back = ptr->back;
		fwd->next = ptr->next;
		// ƒŠƒ“ƒNC³
		p = ptr->back;
		if (p) {
			p->next = fwd;
		}
		else {
			unused = fwd;
		}
		p = ptr->next;
		if (p) {
			p->back = fwd;
		}
	}
	else {
		fwd = ptr->next;
		p = ptr->back;
		if (p) {
			p->next = fwd;
		}
		else {
			unused = fwd;
		}
		p = ptr->next;
		if (p) {
			p->back = ptr->back;
		}
	}
	// ƒŠƒ“ƒN’Ç‰Á
	p = used;
	used = ptr;
	ptr->size = size;
	ptr->lock = 0;
	ptr->back = NULL;
	ptr->next = p;
	if (p) {
		p->back = ptr;
	}
	check("alloc");
#if defined(_MT)
	syscall_mutex_unlock(csec);
#endif
	return((void *)(ptr + 1));
}

static void compaction(MEMHDR self) {

	MEMHDR	next;

	next = self->next;
	if ((next) && (((UINT32)next - (UINT32)self) == self->size)) {
		self->size += next->size;
		next = next->next;
		self->next = next;
		if (next) {
			next->back = self;
		}
	}
}

void memmng_free(void *addr) {

	MEMHDR	cur;
	MEMHDR	ptr;
	MEMHDR	back;
	MEMHDR	next;

#if defined(_MT)
	syscall_mutex_lock(csec);
#endif
	cur = ((MEMHDR)addr) - 1;
	ptr = used;
	while(1) {
		if (ptr == NULL) {
#if defined(_MT)
			syscall_mutex_unlock(csec);
#endif
			return;
		}
		if (ptr == cur) {
			break;
		}
		ptr = ptr->next;
	}
	// ƒŠƒ“ƒNC³
	back = cur->back;
	next = cur->next;
	if (back) {
		back->next = next;
	}
	else {
		used = next;
	}
	if (next) {
		next->back = back;
	}
	// ƒŠƒ“ƒN’Ç‰Á
	back = unused;
	next = NULL;
	while(back) {
		next = back->next;
		if ((next == NULL) || (next > cur)) {
			break;
		}
		back = next;
	}
	if ((back) && (back < cur)) {
		cur->back = back;
		cur->next = next;
		back->next = cur;
		if (next) {
			next->back = cur;
		}
	}
	else {
		next = back;					// ˆê‰ñ–ß‚é
		back = NULL;
		cur->back = back;
		cur->next = next;
		if (next) {
			next->back = cur;
		}
		unused = cur;
	}
	check("free-1");

	// ‚±‚ñ‚Ï‚­‚µ‚å‚ñ
	compaction(cur);
	if (back) {
		compaction(back);
	}

	check("free");
#if defined(_MT)
	syscall_mutex_unlock(csec);
#endif
}

