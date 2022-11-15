// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#if defined( FRANTIC_BUILD_FOR_MAYA )
#include <qglobal.h>
#else
#include <QtCore/qglobal.h>
#endif

#ifndef FRANTIC_BUILD_FOR_MAYA
#if defined( NodeView_EXPORTS )
#define NodeView_API Q_DECL_EXPORT
#else
#define NodeView_API Q_DECL_IMPORT
#endif
#else
// Maya build is static
#define NodeView_API
#endif
