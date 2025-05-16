#pragma once

#define ROUTE_NAME1(a) a
#define ROUTE_NAME2(a, b) a##__##b
#define ROUTE_NAME3(a, b, c) a##__##b##__##c
#define ROUTE_NAME4(a, b, c, d) a##__##b##__##c##__##d
#define ROUTE_NAME5(a, b, c, d, e) a##__##b##__##c##__##d##__##e
#define GET_ROUTE_NAME(_1, _2, _3, _4, _5, NAME, ...) NAME
#define ROUTE_NAME(...) GET_ROUTE_NAME(__VA_ARGS__, ROUTE_NAME5, ROUTE_NAME4, ROUTE_NAME3, ROUTE_NAME2, ROUTE_NAME1)(__VA_ARGS__)


#define ROUTE_STR1(a) #a
#define ROUTE_STR2(a, b) #a "/" #b
#define ROUTE_STR3(a, b, c) #a "/" #b "/" #c
#define ROUTE_STR4(a, b, c, d) #a "/" #b "/" #c "/" #d
#define ROUTE_STR5(a, b, c, d, e) #a "/" #b "/" #c "/" #d "/" #e
#define GET_ROUTE_STR(_1, _2, _3, _4, _5, NAME, ...) NAME
#define ROUTE_STR(...) GET_ROUTE_STR(__VA_ARGS__, ROUTE_STR5, ROUTE_STR4, ROUTE_STR3, ROUTE_STR2, ROUTE_STR1)(__VA_ARGS__)

#define PASTE(a, b) a##b
#define EXPAND(a, b) PASTE(a, b)
