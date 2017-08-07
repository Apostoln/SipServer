def singleton(cls):
    class WrappedClass(cls):
        instance = None
        def __new__(class_, *args, **kwargs):
            if WrappedClass.instance is None:
                WrappedClass.instance = super(WrappedClass, class_).__new__(class_, *args, **kwargs)
                WrappedClass.instance.sealed = False
            return WrappedClass.instance
        def __init__(self, *args, **kwargs):
            if self.sealed:
                return
            super(WrappedClass, self).__init__(*args, **kwargs)
            self.sealed = True
    WrappedClass.__name__ = cls.__name__
    return WrappedClass

