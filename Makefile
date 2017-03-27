# Makefile for NetBSD
# Charlotte Koch <cfkoch@edgebsd.org>
PROG=	vjidi
DPADD+=	${LIBLUA}
LDADD+=	-lm -llua
MKMAN=	no
FILES=	hello.lua
.include <bsd.prog.mk>
