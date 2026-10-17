class Module(object):
    def __init__(self, key, name):
        self.key = key
        self.name = name

class Port(object):
    def __init__(self, key, name, kind, module):
        self.key = key
        self.name = name
        self.kind = kind
        self.module = module

class Connection(object):
    def __init__(self, key, source, target):
        self.key = key
        self.source = source
        self.target = target

map_modules = {}
map_ports = {}
connections = []
m = Module(6,"FileModule")
map_modules[6] = m
m = Module(24,"WorldWindowModule")
map_modules[24] = m
m = Module(0,"IntModule")
map_modules[0] = m
m = Module(21,"ScreenWindowModule")
map_modules[21] = m
m = Module(12,"InfoRenderer")
map_modules[12] = m
m = Module(13,"LonLatModule")
map_modules[13] = m
m = Module(27,"IntModule")
map_modules[27] = m
m = Module(15,"IntModule")
map_modules[15] = m
m = Module(25,"IntModule")
map_modules[25] = m
m = Module(26,"IntModule")
map_modules[26] = m
m = Module(28,"IntModule")
map_modules[28] = m
m = Module(9,"ScreenWindowModule")
map_modules[9] = m
m = Module(18,"MapBordersRenderer")
map_modules[18] = m
m = Module(8,"ScalarFieldDisplayList")
map_modules[8] = m
m = Module(16,"ScalarFieldRenderer")
map_modules[16] = m
m = Module(2,"ScalarFieldData")
map_modules[2] = m
m = Module(1,"OffsetModule")
map_modules[1] = m
m = Module(7,"LonLatGridValue")
map_modules[7] = m
m = Module(19,"MapSelectionsRenderer")
map_modules[19] = m
m = Module(3,"MapLabels")
map_modules[3] = m
m = Module(17,"ScalarFieldRenderer")
map_modules[17] = m
m = Module(14,"ColormapController")
map_modules[14] = m
m = Module(5,"StringModule")
map_modules[5] = m
m = Module(23,"ColormapRenderer")
map_modules[23] = m
m = Module(22,"ScreenWindowModule")
map_modules[22] = m
m = Module(4,"PlotRenderer")
map_modules[4] = m
m = Module(10,"ScreenWindowModule")
map_modules[10] = m
m = Module(11,"CurveData")
map_modules[11] = m
m = Module(20,"TagCloudRenderer")
map_modules[20] = m
p = Port(31,"file", 0, map_modules[12])
map_ports[31] = p
p = Port(71,"int_value", 1, map_modules[26])
map_ports[71] = p
p = Port(73,"int_value", 1, map_modules[28])
map_ports[73] = p
p = Port(34,"lonlat", 1, map_modules[13])
map_ports[34] = p
p = Port(61,"screen_window", 1, map_modules[21])
map_ports[61] = p
p = Port(18,"file", 1, map_modules[6])
map_ports[18] = p
p = Port(72,"int_value", 1, map_modules[27])
map_ports[72] = p
p = Port(41,"int_value", 1, map_modules[15])
map_ports[41] = p
p = Port(70,"int_value", 1, map_modules[25])
map_ports[70] = p
p = Port(69,"world_window", 1, map_modules[24])
map_ports[69] = p
p = Port(17,"string_value", 1, map_modules[5])
map_ports[17] = p
p = Port(0,"int_value", 1, map_modules[0])
map_ports[0] = p
p = Port(32,"screen_window", 0, map_modules[12])
map_ports[32] = p
p = Port(33,"lonlat", 0, map_modules[12])
map_ports[33] = p
p = Port(37,"range", 1, map_modules[14])
map_ports[37] = p
p = Port(35,"default", 0, map_modules[14])
map_ports[35] = p
p = Port(36,"screen_window", 0, map_modules[14])
map_ports[36] = p
p = Port(38,"screen_window", 1, map_modules[14])
map_ports[38] = p
p = Port(39,"default", 1, map_modules[14])
map_ports[39] = p
p = Port(40,"colormap", 1, map_modules[14])
map_ports[40] = p
p = Port(63,"default", 0, map_modules[23])
map_ports[63] = p
p = Port(64,"colormap", 0, map_modules[23])
map_ports[64] = p
p = Port(65,"range", 0, map_modules[23])
map_ports[65] = p
p = Port(66,"screen_window", 0, map_modules[23])
map_ports[66] = p
p = Port(67,"title", 0, map_modules[23])
map_ports[67] = p
p = Port(68,"default", 1, map_modules[23])
map_ports[68] = p
p = Port(62,"screen_window", 1, map_modules[22])
map_ports[62] = p
p = Port(12,"screen_window", 0, map_modules[4])
map_ports[12] = p
p = Port(13,"curves", 0, map_modules[4])
map_ports[13] = p
p = Port(14,"time_indices", 0, map_modules[4])
map_ports[14] = p
p = Port(15,"title", 0, map_modules[4])
map_ports[15] = p
p = Port(16,"file", 0, map_modules[4])
map_ports[16] = p
p = Port(27,"screen_window", 1, map_modules[10])
map_ports[27] = p
p = Port(28,"selection", 0, map_modules[11])
map_ports[28] = p
p = Port(29,"file", 0, map_modules[11])
map_ports[29] = p
p = Port(30,"curve", 1, map_modules[11])
map_ports[30] = p
p = Port(26,"screen_window", 1, map_modules[9])
map_ports[26] = p
p = Port(50,"screen_window", 0, map_modules[18])
map_ports[50] = p
p = Port(51,"world_window", 0, map_modules[18])
map_ports[51] = p
p = Port(52,"map_borders_mode", 0, map_modules[18])
map_ports[52] = p
p = Port(22,"grid", 0, map_modules[8])
map_ports[22] = p
p = Port(23,"colormap", 0, map_modules[8])
map_ports[23] = p
p = Port(24,"interpolation_mode", 0, map_modules[8])
map_ports[24] = p
p = Port(25,"display_list", 1, map_modules[8])
map_ports[25] = p
p = Port(42,"world_window", 0, map_modules[16])
map_ports[42] = p
p = Port(43,"screen_window", 0, map_modules[16])
map_ports[43] = p
p = Port(44,"stencil_flag", 0, map_modules[16])
map_ports[44] = p
p = Port(45,"display_list", 0, map_modules[16])
map_ports[45] = p
p = Port(3,"time_index", 0, map_modules[2])
map_ports[3] = p
p = Port(4,"expression", 0, map_modules[2])
map_ports[4] = p
p = Port(5,"file", 0, map_modules[2])
map_ports[5] = p
p = Port(6,"grid", 1, map_modules[2])
map_ports[6] = p
p = Port(1,"input_int", 0, map_modules[1])
map_ports[1] = p
p = Port(2,"output_int", 1, map_modules[1])
map_ports[2] = p
p = Port(19,"lonlat", 0, map_modules[7])
map_ports[19] = p
p = Port(20,"grid", 0, map_modules[7])
map_ports[20] = p
p = Port(21,"value", 1, map_modules[7])
map_ports[21] = p
p = Port(53,"screen_window", 0, map_modules[19])
map_ports[53] = p
p = Port(54,"world_window", 0, map_modules[19])
map_ports[54] = p
p = Port(55,"selections", 0, map_modules[19])
map_ports[55] = p
p = Port(7,"screen_window", 0, map_modules[3])
map_ports[7] = p
p = Port(8,"time_index", 0, map_modules[3])
map_ports[8] = p
p = Port(9,"file", 0, map_modules[3])
map_ports[9] = p
p = Port(10,"value", 0, map_modules[3])
map_ports[10] = p
p = Port(11,"var_name", 0, map_modules[3])
map_ports[11] = p
p = Port(46,"world_window", 0, map_modules[17])
map_ports[46] = p
p = Port(47,"screen_window", 0, map_modules[17])
map_ports[47] = p
p = Port(48,"lonlat", 0, map_modules[17])
map_ports[48] = p
p = Port(49,"onoff", 0, map_modules[17])
map_ports[49] = p
p = Port(56,"clipping_region", 0, map_modules[20])
map_ports[56] = p
p = Port(57,"tag_clouds", 0, map_modules[20])
map_ports[57] = p
p = Port(58,"black_text", 0, map_modules[20])
map_ports[58] = p
p = Port(59,"title", 0, map_modules[20])
map_ports[59] = p
p = Port(60,"onoff", 0, map_modules[20])
map_ports[60] = p
c = Connection(11, map_ports[27], map_ports[12])
connections.append(c)
c = Connection(39, map_ports[62], map_ports[66])
connections.append(c)
c = Connection(29, map_ports[37], map_ports[65])
connections.append(c)
c = Connection(30, map_ports[40], map_ports[64])
connections.append(c)
c = Connection(28, map_ports[62], map_ports[36])
connections.append(c)
c = Connection(3, map_ports[2], map_ports[14])
connections.append(c)
c = Connection(13, map_ports[18], map_ports[16])
connections.append(c)
c = Connection(14, map_ports[18], map_ports[29])
connections.append(c)
c = Connection(12, map_ports[30], map_ports[13])
connections.append(c)
c = Connection(27, map_ports[34], map_ports[48])
connections.append(c)
c = Connection(33, map_ports[69], map_ports[46])
connections.append(c)
c = Connection(20, map_ports[26], map_ports[47])
connections.append(c)
c = Connection(6, map_ports[6], map_ports[20])
connections.append(c)
c = Connection(0, map_ports[0], map_ports[1])
connections.append(c)
c = Connection(34, map_ports[71], map_ports[49])
connections.append(c)
c = Connection(21, map_ports[26], map_ports[50])
connections.append(c)
c = Connection(16, map_ports[34], map_ports[19])
connections.append(c)
c = Connection(35, map_ports[69], map_ports[51])
connections.append(c)
c = Connection(7, map_ports[6], map_ports[22])
connections.append(c)
c = Connection(17, map_ports[40], map_ports[23])
connections.append(c)
c = Connection(5, map_ports[18], map_ports[5])
connections.append(c)
c = Connection(36, map_ports[72], map_ports[52])
connections.append(c)
c = Connection(4, map_ports[17], map_ports[4])
connections.append(c)
c = Connection(1, map_ports[2], map_ports[3])
connections.append(c)
c = Connection(18, map_ports[41], map_ports[24])
connections.append(c)
c = Connection(26, map_ports[34], map_ports[33])
connections.append(c)
c = Connection(9, map_ports[18], map_ports[9])
connections.append(c)
c = Connection(23, map_ports[26], map_ports[53])
connections.append(c)
c = Connection(10, map_ports[21], map_ports[10])
connections.append(c)
c = Connection(31, map_ports[69], map_ports[42])
connections.append(c)
c = Connection(22, map_ports[26], map_ports[43])
connections.append(c)
c = Connection(37, map_ports[69], map_ports[54])
connections.append(c)
c = Connection(19, map_ports[25], map_ports[45])
connections.append(c)
c = Connection(32, map_ports[70], map_ports[44])
connections.append(c)
c = Connection(8, map_ports[26], map_ports[7])
connections.append(c)
c = Connection(2, map_ports[2], map_ports[8])
connections.append(c)
c = Connection(24, map_ports[26], map_ports[56])
connections.append(c)
c = Connection(38, map_ports[73], map_ports[60])
connections.append(c)
c = Connection(15, map_ports[18], map_ports[31])
connections.append(c)
c = Connection(25, map_ports[61], map_ports[32])
connections.append(c)


modules = map_modules.values()
ports   = map_ports.values()
