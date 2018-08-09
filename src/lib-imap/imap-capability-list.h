#ifndef IMAP_CAPABILITY_LIST_H
#define IMAP_CAPABILITY_LIST_H

#include <stdint.h>
#include <stddef.h>

#include "lib.h"
#include "array.h"

/* bitflags to represent different conditions of
   the current imap session */
enum imap_capability_visibility {
	IMAP_CAP_VISIBILITY_NEVER           = 0,      /* never show this capability */
	IMAP_CAP_VISIBILITY_PREAUTH         = BIT(0), /* show this cap before login */
	IMAP_CAP_VISIBILITY_POSTAUTH        = BIT(1), /* show this cap after login */
	IMAP_CAP_VISIBILITY_TLS_ACTIVE      = BIT(2), /* show this cap if tls is active */
	IMAP_CAP_VISIBILITY_TLS_INACTIVE    = BIT(3), /* show this cap if tls is inactive */
	IMAP_CAP_VISIBILITY_NO_LOGIN        = BIT(6), /* show this cap if login is disabled */
	/* NOTE: connections from localhost are 'secure' regardless of SSL/TLS! */
	IMAP_CAP_VISIBILITY_SECURE          = BIT(7), /* show this cap if we are secured */
	IMAP_CAP_VISIBILITY_INSECURE        = BIT(8), /* show this cap if we aren't secured */

	/* IMAP_CAP_VISIBILITY_REQUIRE_ALL indicates that the rest
	   of the conditions must be met *exactly* to show this
	   capability, otherwise if *at least* 1 condition is met
	   the capability will be shown */
	IMAP_CAP_VISIBILITY_REQUIRE_ALL     = BIT(31),

	IMAP_CAP_VISIBILITY_ALWAYS          = 0x7FFFFFFF /* always show this cap */
};

/*  the initial size of the capability list
	it gets automatically increased but this
	is the size that we start at */
#define INITIAL_CAP_LIST_SIZE 5
/*  The amount to grow the capability list
	each time it fills up */
#define CAP_LIST_GROW_AMOUNT  1

/*  This structure describes a capability word that appears
	in the imap and imap-login capability string. The flags
	indicate when and under which conditions the capability
	word should be displayed */
struct imap_capability {
	char *name;
	enum imap_capability_visibility flags;
};

/* define our array type for the imap capability array */
ARRAY_DEFINE_TYPE(imap_capability_array, struct imap_capability);

/* define the structure that holds the capability pool and capability array */
struct imap_capability_list {
	/* reference count */
	int refcount;
	/* imap capability list memory pool */
	pool_t pool;
	/* array of imap_capability objects */
	ARRAY_TYPE(imap_capability_array) imap_capabilities;
};

/* initialize the imap capability list */
struct imap_capability_list *
imap_capability_list_create(const char *initial_entries);
/* increment reference to an imap capability list */
void imap_capability_list_ref(struct imap_capability_list *capability_list);
/* decrement reference to an imap capability list */
void imap_capability_list_unref(struct imap_capability_list **capability_list);
/* add an entry to the imap capability list */
void imap_capability_list_add(struct imap_capability_list *capability_list,
		const char *name, enum imap_capability_visibility flags);
/* remove an entry from the imap capability list */
void imap_capability_list_remove(struct imap_capability_list *capability_list,
		const char *name);
/* split a string by space and add each entry to the imap capability list */
void imap_capability_list_append_string(struct imap_capability_list *capability_list,
		const char *name_list);
/* find an imap capability by name */
struct imap_capability *imap_capability_list_find(struct imap_capability_list *capability_list,
		const char *name);
/* append the capabilities to cap_str based on given IMAP_CAP_VISIBILITY_ flags */
void imap_capability_list_get_capability(struct imap_capability_list *capability_list, 
		string_t *cap_str, uint32_t flags);

#endif
