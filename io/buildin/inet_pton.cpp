#include <errno.h>
#include <lemon/io/buildin/inet_pton.h>
#include <lemon/sys/text.h>

#ifndef LEMON_HAS_INET_PTON

LEMON_IO_PRIVATE int inet_pton_v4(const char *src, void *dst)
{
	static const char digits[] = "0123456789";

	lemon_byte_t bytes[sizeof(in_addr)] = {0};

	lemon_byte_t * val = bytes;

	int index = 0;

	bool status = false;

	for(char iter = *src; iter != 0; iter = * (++src)){

		const char *found = strchr(digits,iter);

		if(index == 4) goto Error;

		if(found){
			int digit = (*val) * 10 + (int)(found - digits);

			if(digit > 255) goto Error;

			status = true;

			*val = (lemon_byte_t)digit;

		}else if(iter == '.' && status){

			*++val = 0;

			status = false;

			++ index;

		}else{

			goto Error;
		}
	}

	memcpy(dst,bytes,sizeof(bytes));

	return 1;

Error:
	errno = EAFNOSUPPORT;

	return 0;
}
#ifdef LEMON_SUPPORT_IPV6



LEMON_IO_PRIVATE int inet_pton_v6(const char *src, void *dst)
{
	static const char xdigits_l[] = "0123456789abcdef",
		xdigits_u[] = "0123456789ABCDEF";
	u_char tmp[sizeof(in_addr6)], *tp, *endp, *colonp;
	const char *xdigits, *curtok;
	int ch, saw_xdigit;
	u_int val;

	memset((tp = tmp), '\0', sizeof(in_addr6));
	endp = tp + sizeof(in_addr6);
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return (0);
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = *src++) != '\0') {
		const char *pch;

		if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
			pch = strchr((xdigits = xdigits_u), ch);
		if (pch != NULL) {
			val <<= 4;
			val |= (pch - xdigits);
			if (val > 0xffff)
				return (0);
			saw_xdigit = 1;
			continue;
		}
		if (ch == ':') {
			curtok = src;
			if (!saw_xdigit) {
				if (colonp)
					return (0);
				colonp = tp;
				continue;
			} else if (*src == '\0') {
				return (0);
			}
			if (tp + sizeof(short) > endp)
				return (0);
			*tp++ = (u_char) (val >> 8) & 0xff;
			*tp++ = (u_char) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + sizeof(in_addr)) <= endp) &&
			inet_pton_v4(curtok, tp) > 0) {
				tp += sizeof(in_addr);
				saw_xdigit = 0;
				break;	/* '\0' was seen by inet_pton4(). */
		}
		return (0);
	}
	if (saw_xdigit) {
		if (tp + sizeof(short) > endp)
			return (0);
		*tp++ = (u_char) (val >> 8) & 0xff;
		*tp++ = (u_char) val & 0xff;
	}
	if (colonp != NULL) {
		/*
		* Since some memmove()'s erroneously fail to handle
		* overlapping regions, we'll do the shift by hand.
		*/
		const int n = (int)(tp - colonp);
		int i;

		if (tp == endp)
			return (0);
		for (i = 1; i <= n; i++) {
			endp[- i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return (0);
	memcpy(dst, tmp, sizeof(in_addr6));
	return (1);

}
#endif //#ifdef LEMON_SUPPORT_IPV6
LEMON_IO_PRIVATE int inet_pton(int af, const char *src, void *dst)
{
	switch(af)
	{
	case AF_INET:
		{
			return inet_pton_v4(src,dst);
		}
#ifdef LEMON_SUPPORT_IPV6
	case AF_INET6:
		{
			return inet_pton_v6(src,dst);
		}
#endif //LEMON_SUPPORT_IPV6
	default:
		{
			errno = EAFNOSUPPORT;

			return -1;
		}
	};
}

#endif //LEMON_HAS_INET_PTON