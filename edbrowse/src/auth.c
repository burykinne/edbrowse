/* auth.c
 * user password authorization for web access
 * (c) 2002 Mikulas Patocka
 * This file is part of the Links project, released under GPL.
 *
 * Modified by Karl Dahlke for integration with edbrowse.
 */

#include "eb.h"

struct httpAuth {
	struct httpAuth *next;
	struct httpAuth *prev;
/* These strings are allocated. */
	char *host;
	char *directory;
	char *realm;
	char *user_password;
	int port;
	bool proxy;
};

static struct listHead authlist = { &authlist, &authlist };

bool getUserPass(const char *url, char *creds, bool find_proxy)
{
	int port = getPortURL(url);
	char host[MAXHOSTLEN];
	const char *dir, *dirend;
	struct httpAuth *a;
	struct httpAuth *found = NULL;
	int d1len, d2len;

	if (!getProtHostURL(url, NULL, host))
		return false;

	getDirURL(url, &dir, &dirend);
	d2len = dirend - dir;

	foreach(a, authlist) {
		if (found == NULL && a->proxy == find_proxy &&
		    stringEqualCI(a->host, host) && a->port == port) {
			if (!a->proxy) {
/* Directory match not done for proxy records. */
				d1len = strlen(a->directory);
				if (d1len > d2len)
					continue;
				if (memcmp(a->directory, dir, d1len))
					continue;
				found = a;
			} else	/* not proxy */
				found = a;
		}
	}

	if (found)
		strcpy(creds, found->user_password);

	return (found != NULL);
}				/* getUserPass */

bool getUserPassRealm(const char *url, char *creds, const char *realm)
{
	char host[MAXHOSTLEN];
	int port = getPortURL(url);
	struct httpAuth *a;
	struct httpAuth *found = NULL;

	if (!getProtHostURL(url, NULL, host))
		return false;

	foreach(a, authlist) {
		if (found == NULL && stringEqualCI(a->host, host) &&
		    a->port == port) {
			if (!a->realm)
				continue;
			if (strcmp(a->realm, realm))
				continue;
			found = a;
		}
	}

	if (found)
		strcpy(creds, found->user_password);

	return (found != NULL);
}

bool
addWebAuthorization(const char *url,
		    const char *credentials, bool proxy, const char *realm)
{
	char host[MAXHOSTLEN];
	struct httpAuth *a;
	const char *dir = 0, *dirend;
	int port, dl;
	bool urlProx = isProxyURL(url);
	bool updated = false;

	if (proxy) {
		if (!urlProx) {
			setError(MSG_ProxyAuth);
			return false;
		}
	} else if (urlProx)
		url = getDataURL(url);

	if (!getProtHostURL(url, NULL, host))
		return false;
	port = getPortURL(url);
	if (!proxy) {
		getDirURL(url, &dir, &dirend);
		dl = dirend - dir;
	}

/* See if we've done this one before. */
	foreach(a, authlist) {
		if (a->proxy == proxy &&
		    a->port == port &&
		    stringEqualCI(a->host, host) &&
		    (proxy || (dl == strlen(a->directory)
			       && !memcmp(a->directory, dir, dl)))) {
			char *s = cloneString(credentials);
			char *t = a->user_password;
			a->user_password = s;
			nzFree(t);
			updated = true;
			break;
		}
	}

	if (!updated) {
		a = allocZeroMem(sizeof(struct httpAuth));
		a->proxy = proxy;
		a->port = port;
		if (!a->host)
			a->host = cloneString(host);
		if (dir && !a->directory)
			a->directory = pullString1(dir, dirend);
		if (realm && !a->realm)
			a->realm = cloneString(realm);
		a->user_password = cloneString(credentials);
		addToListFront(&authlist, a);
	}

	debugPrint(3, "%s authorization for %s%s",
		   updated ? "updated" : "new", a->host, a->directory);
	return true;
}				/* addWebAuthorization */
