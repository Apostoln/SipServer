from singleton import singleton


@singleton
class Config:
    address = None
    port = None
    path = None
    interface = None
    DEBUG = None
    serverEndPoint = None