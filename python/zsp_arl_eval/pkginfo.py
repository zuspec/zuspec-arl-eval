import os
import ivpm

class PkgInfo(ivpm.PkgInfo):

    def __init__(self):
        pkgdir = os.path.dirname(os.path.abspath(__file__))
        projdir = os.path.dirname(os.path.dirname(pkgdir))
        super().__init__("zsp-arl-eval", os.path.dirname(pkgdir))

        if os.path.isdir(os.path.join(projdir, "src")):
            self._incdirs = [
                os.path.join(projdir, "src", "include"),
                os.path.join(projdir, "python")]
            self._libdirs = [
                os.path.join(projdir, "build", "lib"),
                os.path.join(projdir, "build", "lib64")]
        else:
            self._incdirs = [os.path.join(pkgdir, "share", "include")]
            self._libdirs = [os.path.join(pkgdir)]

        self._deps = ["zsp-arl-dm", "vsc-solvers"]

        self._libs = ["zsp-arl-eval"]
