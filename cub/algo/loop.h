// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HC8777C2D_A6D2_4569_B639_4B3B68B935C6
#define HC8777C2D_A6D2_4569_B639_4B3B68B935C6

// NOTE: Enumeration Constants should cast to integer explicitly.
#define FOREACH_FROM(i, from, end) for (auto i = static_cast<decltype(end)>(from); i != end; ++i)

#define FOREACH(i, max) FOREACH_FROM(i, 0, max)
#define FOREVER while (true)

#endif
