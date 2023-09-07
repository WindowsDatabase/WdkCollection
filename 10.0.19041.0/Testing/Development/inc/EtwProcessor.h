/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*
*/
#pragma once

#ifdef ETWPROCESSOR_EXPORTS
# define ETWPROCESSOR_API __declspec(dllexport)
#elif defined(ETWPROCESSOR_STATIC)
# define ETWPROCESSOR_API
#else
# define ETWPROCESSOR_API __declspec(dllimport)
#endif