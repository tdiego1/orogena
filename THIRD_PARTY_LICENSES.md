# Third-Party Licenses

Orogena uses the following open-source libraries. We are grateful to their authors and contributors.

---

## Qt 6

**License:** GNU Lesser General Public License v3.0 (LGPL-3.0)
**Website:** https://www.qt.io/
**Usage:** GUI framework, OpenGL integration, SQL database access

Orogena dynamically links to Qt libraries in compliance with LGPL-3.0 requirements. Users can replace Qt libraries with different versions compatible with Qt 6.8+.

**LGPL-3.0 Summary:**
- Qt is licensed under LGPL v3, allowing use in both open-source and proprietary applications
- Applications must dynamically link Qt libraries (not static linking)
- Users must be able to replace Qt libraries with compatible versions
- Modifications to Qt itself must be shared under LGPL

Full license text: https://www.gnu.org/licenses/lgpl-3.0.html

---

## GLM (OpenGL Mathematics)

**License:** MIT License
**Website:** https://github.com/g-truc/glm
**Usage:** Vector and matrix mathematics for 3D rendering

```
The MIT License (MIT)

Copyright (c) 2005 - G-Truc Creation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## spdlog

**License:** MIT License
**Website:** https://github.com/gabime/spdlog
**Usage:** Fast C++ logging library

```
The MIT License (MIT)

Copyright (c) 2016 Gabi Melman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## JSON for Modern C++ (nlohmann-json)

**License:** MIT License
**Website:** https://github.com/nlohmann/json
**Usage:** JSON parsing and serialization

```
MIT License

Copyright (c) 2013-2022 Niels Lohmann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## libassert

**License:** MIT License
**Website:** https://github.com/jeremy-rifkin/libassert
**Usage:** Enhanced assertion library

```
MIT License

Copyright (c) 2021-2024 Jeremy Rifkin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## SQLite

**License:** Public Domain
**Website:** https://www.sqlite.org/
**Usage:** Embedded SQL database engine

SQLite is in the public domain and may be used for any purpose without restriction.

From the SQLite website:
> "SQLite is in the public domain and does not require a license."

Full details: https://www.sqlite.org/copyright.html

---

## Google Test (GoogleTest)

**License:** BSD 3-Clause License
**Website:** https://github.com/google/googletest
**Usage:** Unit testing framework (development/testing only, not distributed)

```
BSD 3-Clause License

Copyright (c) 2008, Google Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

---

## OpenGL

**Type:** API Specification (not a library)
**Website:** https://www.opengl.org/
**Usage:** 3D graphics rendering API

OpenGL is an open standard maintained by the Khronos Group. The OpenGL API itself is not subject to licensing restrictions. OpenGL implementations are provided by graphics card drivers (Mesa, NVIDIA, AMD, Intel) under their respective licenses.

---

## License Compatibility

All third-party libraries used in Orogena are compatible with GPL-3.0:

| Library | License | GPL-3.0 Compatible? |
|---------|---------|---------------------|
| Qt 6 | LGPL-3.0 | ✅ Yes (when dynamically linked) |
| GLM | MIT | ✅ Yes |
| spdlog | MIT | ✅ Yes |
| nlohmann-json | MIT | ✅ Yes |
| libassert | MIT | ✅ Yes |
| SQLite | Public Domain | ✅ Yes |
| GoogleTest | BSD-3-Clause | ✅ Yes |

**Note:** MIT, BSD, and Public Domain licenses are permissive and fully compatible with GPL-3.0. LGPL-3.0 is compatible when dynamically linking (which Orogena does).

---

## Attribution

If you distribute Orogena or derivative works, you must:

1. **Include this file** (THIRD_PARTY_LICENSES.md) or equivalent attribution
2. **Comply with LGPL-3.0** for Qt (ensure dynamic linking, don't static link without Qt Commercial license)
3. **Include copyright notices** for MIT/BSD libraries (automatically satisfied by including this file)

---

**Last Updated:** 2026-01-02
