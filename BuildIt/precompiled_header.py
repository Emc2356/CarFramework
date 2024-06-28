from .source_file import SourceFile


class PreCompiledHeader:
    __slots__ = ("source", "extra_build_flags", "extra_defines")

    def __init__(self, source, extra_build_flags, extra_defines):
        self.source = SourceFile.from_list([source])[0]
        self.extra_build_flags = extra_build_flags
        self.extra_defines = {define[0]: (define[1] if len(define) == 2 else None) for define in extra_defines}

        # shitty ass circular imports :(
        from .register import Register
        
        Register.submit(self)
