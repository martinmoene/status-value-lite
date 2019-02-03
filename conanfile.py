from conans import ConanFile

class StatusValueLiteConan(ConanFile):
    version = "1.0.0"
    name = "status-value-lite"
    description = "A class for status and optional value for C++11 and later, C++98 variant provided in a single-file  header-only library"
    license = "Boost Software License - Version 1.0. http://www.boost.org/LICENSE_1_0.txt"
    url = "https://github.com/martinmoene/status-value-lite.git"
    exports_sources = "include/nonstd/*", "LICENSE.txt"
    build_policy = "missing"    
    author = "Martin Moene"

    def build(self):
        """Avoid warning on build step"""
        pass

    def package(self):
        """Provide pkg/include/nonstd/*.hpp"""
        self.copy("*.hpp")

    def package_info(self):
        self.info.header_only()
