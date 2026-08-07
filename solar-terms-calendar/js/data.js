const TIAN_GAN = ['甲', '乙', '丙', '丁', '戊', '己', '庚', '辛', '壬', '癸'];
const TIAN_GAN_WUXING = ['木', '木', '火', '火', '土', '土', '金', '金', '水', '水'];
const TIAN_GAN_YINYANG = ['阳', '阴', '阳', '阴', '阳', '阴', '阳', '阴', '阳', '阴'];

const DI_ZHI = ['子', '丑', '寅', '卯', '辰', '巳', '午', '未', '申', '酉', '戌', '亥'];
const DI_ZHI_WUXING = ['水', '土', '木', '木', '土', '火', '火', '土', '金', '金', '土', '水'];
const DI_ZHI_SHENGXIAO = ['鼠', '牛', '虎', '兔', '龙', '蛇', '马', '羊', '猴', '鸡', '狗', '猪'];

const SHI_CHEN = [
  { name: '子时', range: '23:00-01:00', diZhi: '子', animal: '鼠', description: '夜半，又名子夜、中夜。十二时辰的第一个时辰。夜深人静，万物休眠，阴气最盛之时。' },
  { name: '丑时', range: '01:00-03:00', diZhi: '丑', animal: '牛', description: '鸡鸣，又名荒鸡。十二时辰的第二个时辰。黑夜即将过去，黎明前的黑暗时分。' },
  { name: '寅时', range: '03:00-05:00', diZhi: '寅', animal: '虎', description: '平旦，又称黎明、早晨、日旦。夜与日的交替之际，老虎在此时最猛。' },
  { name: '卯时', range: '05:00-07:00', diZhi: '卯', animal: '兔', description: '日出，又名日始、破晓、旭日。太阳冉冉升起，兔子出窝吃草。' },
  { name: '辰时', range: '07:00-09:00', diZhi: '辰', animal: '龙', description: '食时，又名早食。古人"朝食"之时，传说龙在此时行云布雨。' },
  { name: '巳时', range: '09:00-11:00', diZhi: '巳', animal: '蛇', description: '隅中，又名日禺。临近中午，大雾散去，蛇开始活动。' },
  { name: '午时', range: '11:00-13:00', diZhi: '午', animal: '马', description: '日中，又名日正、中午。阳气最盛之时，太阳正当空，是午休的好时光。' },
  { name: '未时', range: '13:00-15:00', diZhi: '未', animal: '羊', description: '日昳，又名日跌、日央。太阳偏西，羊群此时吃草最为适宜。' },
  { name: '申时', range: '15:00-17:00', diZhi: '申', animal: '猴', description: '哺时，又名日哺、夕食。猴子喜欢在此时啼叫，天气渐凉。' },
  { name: '酉时', range: '17:00-19:00', diZhi: '酉', animal: '鸡', description: '日入，又名日落、日沉、傍晚。夕阳西下，鸡开始归巢。' },
  { name: '戌时', range: '19:00-21:00', diZhi: '戌', animal: '狗', description: '黄昏，又名日夕、日暮、日晚。天色将黑，狗开始守夜看家。' },
  { name: '亥时', range: '21:00-23:00', diZhi: '亥', animal: '猪', description: '人定，又名定昏。夜色已深，人们停止活动，安歇睡眠，猪睡得最香。' }
];

const SI_JI = [
  {
    name: '春', color: '#4CAF50', icon: '🌸',
    months: [2, 3, 4],
    solarTerms: ['立春', '雨水', '惊蛰', '春分', '清明', '谷雨'],
    description: '春为四季之首，万物复苏，生机勃勃。此时阳气上升，阴气下降，天地交泰，万象更新。春季养生重在养肝护肝，保持心情舒畅。',
    healthTips: '春捂秋冻，保暖避风；早睡早起，舒展筋骨；饮食清淡，多食绿色蔬菜。'
  },
  {
    name: '夏', color: '#F44336', icon: '☀️',
    months: [5, 6, 7],
    solarTerms: ['立夏', '小满', '芒种', '夏至', '小暑', '大暑'],
    description: '夏为阳盛之时，万物繁茂，生机盎然。阳气最旺，暑气当令。夏季养生重在养心，保持心神安宁。',
    healthTips: '避暑降温，适当午睡；饮食清淡，多食苦味；心静自然凉，避免暴怒。'
  },
  {
    name: '秋', color: '#FF9800', icon: '🍂',
    months: [8, 9, 10],
    solarTerms: ['立秋', '处暑', '白露', '秋分', '寒露', '霜降'],
    description: '秋为收敛之季，万物成熟，果实累累。阳气渐收，阴气渐长。秋季养生重在养肺润燥。',
    healthTips: '早卧早起，与鸡俱兴；润肺养阴，多食白色食物；收敛神气，使志安宁。'
  },
  {
    name: '冬', color: '#2196F3', icon: '❄️',
    months: [11, 12, 1],
    solarTerms: ['立冬', '小雪', '大雪', '冬至', '小寒', '大寒'],
    description: '冬为闭藏之季，万物潜藏，蓄势待发。阴气最盛，阳气潜藏。冬季养生重在养肾藏精。',
    healthTips: '早卧晚起，必待日光；保暖御寒，温补肾阳；多食黑色食物，进补适时。'
  }
];

const SOLAR_TERMS = [
  {
    name: '立春', pinyin: 'Lì Chūn', english: 'Start of Spring',
    season: '春', month: 2, day: 4,
    description: '立春是二十四节气之首，标志着春季的开始。"立"是开始的意思，春代表着温暖和生长。从这一天起，寒冷的冬天即将过去，万物开始复苏，风和日暖，鸟语花香。',
    background: '立春源于周代，古代帝王在立春这天要率文武百官举行迎春大典，祈求风调雨顺、五谷丰登。民间有"打春牛"的习俗，用泥土塑成春牛鞭打，寓意催耕迎春。',
    dietary: [
      { food: '春饼', reason: '咬春习俗，薄饼卷时令蔬菜，象征迎接春天' },
      { food: '萝卜', reason: '通气消食，民间有"咬春"吃萝卜的习俗' },
      { food: '韭菜', reason: '韭菜性温，助阳气升发，春季时令蔬菜' },
      { food: '春笋', reason: '清脆鲜嫩，通利九窍，是立春时令美食' }
    ],
    climate: '东风解冻，阳气回升，乍暖还寒',
    custom: '打春牛、咬春、贴宜春帖、游春',
    poems: '律回岁晚冰霜少，春到人间草木知。——宋·张栻《立春偶成》'
  },
  {
    name: '雨水', pinyin: 'Yǔ Shuǐ', english: 'Rain Water',
    season: '春', month: 2, day: 19,
    description: '雨水节气意味着降雨开始增多，气温回升，冰雪融化，春雨润物细无声。从此，大地渐渐开始呈现出一派欣欣向荣的景象。',
    background: '雨水节气反映了降水现象。古语说"春雨贵如油"，此时正是春耕备耕的关键时期，适时的雨水对农作物的生长至关重要。民间有"雨水节，回娘家"的习俗。',
    dietary: [
      { food: '红枣粥', reason: '补血养气，健脾养胃，去湿气' },
      { food: '蜂蜜', reason: '润肺止咳，春季养生的佳品' },
      { food: '菠菜', reason: '富含铁质，春季时令，养血润燥' },
      { food: '山药', reason: '健脾祛湿，适合雨水时节养生' }
    ],
    climate: '气温回升，冰雪融化，降水增多',
    custom: '回娘家、接寿、拉保保',
    poems: '好雨知时节，当春乃发生。——唐·杜甫《春夜喜雨》'
  },
  {
    name: '惊蛰', pinyin: 'Jīng Zhé', english: 'Awakening of Insects',
    season: '春', month: 3, day: 6,
    description: '惊蛰时节，春雷始鸣，惊醒蛰伏于地下越冬的昆虫。"蛰"是藏的意思。此时，天气转暖，春雷滚动，万物生机盎然。',
    background: '古人认为春雷惊醒了冬眠的动物，故名惊蛰。实际上，是大地回春，天气变暖使动物结束冬眠。民间有惊蛰吃梨的习俗，寓意远离疾病，保一年健康。',
    dietary: [
      { food: '梨', reason: '润肺清热，惊蛰吃梨寓意远离疾病' },
      { food: '蜂蜜雪梨水', reason: '滋阴润燥，缓解春燥咳嗽' },
      { food: '茼蒿', reason: '清肝明目，春季养肝之选' },
      { food: '鸡蛋', reason: '补充蛋白质，"惊蛰吃蛋，气力多一万"' }
    ],
    climate: '春雷乍动，雨水增多，气温回升明显',
    custom: '吃梨、祭白虎、打小人、蒙鼓皮',
    poems: '微雨众卉新，一雷惊蛰始。——唐·韦应物《观田家》'
  },
  {
    name: '春分', pinyin: 'Chūn Fēn', english: 'Spring Equinox',
    season: '春', month: 3, day: 21,
    description: '春分这天，太阳直射赤道，全球各地昼夜等长。"分"是平分的意思，春分不仅平分了春季，还平分了昼夜。春分过后，北半球白昼渐长，黑夜渐短。',
    background: '春分在古代是重要的节日，也是祭祀日。《礼记》记载"春分祭日"，帝王春分祭日，秋分祭月。民间有竖蛋的习俗，"春分到，蛋儿俏"。',
    dietary: [
      { food: '春菜', reason: '"春分吃春菜"，野菜嫩绿，清热解毒' },
      { food: '豆芽', reason: '富含维生素，助阳气生发' },
      { food: '枸杞叶', reason: '养肝明目，春季时令之选' },
      { food: '鲫鱼', reason: '营养丰富，健脾利湿，春分食补佳品' }
    ],
    climate: '昼夜等长，气温稳定回升，莺飞草长',
    custom: '竖蛋、祭日、踏青、放风筝',
    poems: '春分雨脚落声微，柳岸斜风带客归。——宋·徐铉《春分日》'
  },
  {
    name: '清明', pinyin: 'Qīng Míng', english: 'Clear and Bright',
    season: '春', month: 4, day: 5,
    description: '清明节是重要的传统节日，也是唯一一个既是节气又是节日的日子。此时，天气清澈明朗，万物"清洁明净"。人们祭祖扫墓、踏青赏春。',
    background: '清明节融合了寒食节和上巳节的习俗，距今已有2500多年历史。晋文公为纪念介子推而设立的寒食禁火习俗，逐渐演变为清明扫墓祭祖的传统。',
    dietary: [
      { food: '青团', reason: '用艾草汁拌糯米粉制成，是清明传统美食' },
      { food: '清明螺', reason: '"清明螺，赛过鹅"，此时螺蛳最为肥美' },
      { food: '荠菜', reason: '荠菜当灵丹，清肝明目、利水消肿' },
      { food: '馓子', reason: '古代寒食节食品，酥脆可口' }
    ],
    climate: '气清景明，万物皆显，雨纷纷',
    custom: '扫墓祭祖、踏青、插柳、放风筝、荡秋千',
    poems: '清明时节雨纷纷，路上行人欲断魂。——唐·杜牧《清明》'
  },
  {
    name: '谷雨', pinyin: 'Gǔ Yǔ', english: 'Grain Rain',
    season: '春', month: 4, day: 20,
    description: '谷雨是春季最后一个节气，取自"雨生百谷"之意。此时降水明显增加，田中的秧苗初插、作物新种，最需要雨水的滋润。谷雨过后便是立夏，春天即将过去。',
    background: '传说仓颉造字成功之日，"天雨粟，鬼夜哭"，因此谷雨也有纪念仓颉的习俗。谷雨采茶是重要的民俗活动，谷雨茶清火明目，深受喜爱。',
    dietary: [
      { food: '谷雨茶', reason: '谷雨当天采摘的茶叶，清火明目辟邪' },
      { food: '香椿', reason: '"雨前香椿嫩如丝"，谷雨前后最为鲜美' },
      { food: '黑豆', reason: '健脾利湿，补肾益阴，适合暮春食用' },
      { food: '桑葚', reason: '滋阴补血，润肠通便，谷雨时令鲜果' }
    ],
    climate: '雨量充足及时，气温升高，柳絮飞落',
    custom: '采谷雨茶、食香椿、赏牡丹、祭仓颉',
    poems: '杨花落尽子规啼，闻道龙标过五溪。——唐·李白《闻王昌龄左迁龙标遥有此寄》'
  },
  {
    name: '立夏', pinyin: 'Lì Xià', english: 'Start of Summer',
    season: '夏', month: 5, day: 6,
    description: '立夏是夏季的开始，"立"是开始的意思。此时，万物生长旺盛，气温明显升高，雷雨增多。立夏时节，夏收作物进入生长后期。',
    background: '古代帝王在立夏这天要率百官举行迎夏仪式，以表达对丰收的企盼。民间有"立夏称人"的习俗，称体重以祈求健康。还有吃蛋、斗蛋的习惯。',
    dietary: [
      { food: '鸡蛋', reason: '"立夏吃蛋，石头踩烂"，补充营养抵抗苦夏' },
      { food: '豌豆', reason: '立夏尝新，豌豆鲜嫩，益气健脾' },
      { food: '樱桃', reason: '立夏见三鲜之一，补血养颜' },
      { food: '苦瓜', reason: '清心降火，适合夏季食用，预防苦夏' }
    ],
    climate: '气温明显升高，雷雨增多，万物繁茂',
    custom: '称人、吃蛋、斗蛋、吃立夏饭',
    poems: '绿树阴浓夏日长，楼台倒影入池塘。——唐·高骈《山亭夏日》'
  },
  {
    name: '小满', pinyin: 'Xiǎo Mǎn', english: 'Grain Full',
    season: '夏', month: 5, day: 21,
    description: '小满的含义是夏熟作物的籽粒开始灌浆饱满，但还未成熟，只是"小满"，尚未"大满"。此时，麦类等夏熟作物籽粒开始饱满，大地一片生机。',
    background: '小满是收获的前奏，也是炎热夏季的开始。古谚云"小满不满，芒种不管"，意思是雨水要足够才能有好收成。南方有"小满动三车"的习俗（水车、油车、丝车）。',
    dietary: [
      { food: '苦菜', reason: '"小满食苦"，苦菜清热泻火，防治热症' },
      { food: '薏仁', reason: '健脾祛湿，适合小满湿气重的天气' },
      { food: '枇杷', reason: '小满时节枇杷成熟，润肺止咳' },
      { food: '黄瓜', reason: '清热解暑，水分充足，夏季必备蔬菜' }
    ],
    climate: '降雨增多，湿热渐盛，夏熟作物饱满',
    custom: '祭车神、食苦菜、看麦熟',
    poems: '小满田塍寻草药，农闲莫问动三车。——宋·欧阳修《小满》'
  },
  {
    name: '芒种', pinyin: 'Máng Zhòng', english: 'Grain in Ear',
    season: '夏', month: 6, day: 6,
    description: '芒种的意思是"有芒的麦子快收，有芒的稻子可种"。芒种是农民播种、收割最繁忙的时候，故又称"忙种"。此时中国大部分地区农业生产进入"三夏"大忙季节。',
    background: '芒种在小满和夏至之间，是一个转折期。古代有"送花神"的习俗，芒种过后百花开始凋零，人们会举行送花神仪式。还有煮青梅酒的习俗。',
    dietary: [
      { food: '青梅', reason: '芒种煮梅，青梅酿酒，生津止渴' },
      { food: '鸭肉', reason: '性凉味甘，滋阴养胃，适合夏季进补' },
      { food: '西瓜', reason: '清热解暑，补充水分，夏季消暑圣品' },
      { food: '绿豆汤', reason: '清热解毒，消暑止渴，芒种必备饮品' }
    ],
    climate: '雨量充沛，气温显著升高，潮湿闷热',
    custom: '送花神、煮梅、安苗、打泥巴仗',
    poems: '时雨及芒种，四野皆插秧。——宋·陆游《时雨》'
  },
  {
    name: '夏至', pinyin: 'Xià Zhì', english: 'Summer Solstice',
    season: '夏', month: 6, day: 21,
    description: '夏至是北半球一年中白昼最长的一天，太阳直射北回归线。"至"是极致的意思。夏至过后，白昼渐短，黑夜渐长。古人认为夏至是"阴阳相争"的时节。',
    background: '夏至是二十四节气中最早被确定的一个节气，可追溯到公元前七世纪，古人用土圭测日影确定了夏至。古代夏至要举行祭祀，以祈求消灾丰收。',
    dietary: [
      { food: '面条', reason: '"冬至饺子夏至面"，凉面清爽开胃' },
      { food: '苦瓜', reason: '夏至吃苦，清心降火，养心安神' },
      { food: '莲子', reason: '养心安神，补脾止泻，夏至养心佳品' },
      { food: '绿豆芽', reason: '清热解毒，利水消肿，清爽可口' }
    ],
    climate: '白昼最长，天气炎热，雷阵雨频繁',
    custom: '祭祖、吃面、称重、避伏',
    poems: '昼晷已云极，宵漏自此长。——唐·韦应物《夏至避暑北池》'
  },
  {
    name: '小暑', pinyin: 'Xiǎo Shǔ', english: 'Minor Heat',
    season: '夏', month: 7, day: 7,
    description: '小暑意指天气开始炎热，但还没到最热的时候。"暑"是炎热的意思。江淮流域"梅雨"季节即将结束，盛夏开始，气温升高，进入伏旱期。',
    background: '小暑是人体阳气最旺盛的时候，"春夏养阳"，人们要保护阳气。民间有"小暑大暑，上蒸下煮"之说。还有"食新"的习俗，小暑过后尝新米。',
    dietary: [
      { food: '黄鳝', reason: '"小暑黄鳝赛人参"，滋补强身' },
      { food: '蜜汁藕', reason: '清热凉血，健脾开胃，适合夏季食用' },
      { food: '生姜', reason: '"冬吃萝卜夏吃姜"，小暑吃姜祛湿暖胃' },
      { food: '西瓜皮', reason: '清热解暑，利尿消肿，变废为宝' }
    ],
    climate: '暑气上升，高温潮湿，时有暴雨',
    custom: '食新、吃黄鳝、晒书画、晒衣物',
    poems: '倏忽温风至，因循小暑来。——唐·元稹《小暑六月节》'
  },
  {
    name: '大暑', pinyin: 'Dà Shǔ', english: 'Major Heat',
    season: '夏', month: 7, day: 23,
    description: '大暑是一年中最热的时期，"大暑乃炎热之极也"。此时，气温高，农作物生长最快。同时，旱、涝、风灾也最为频繁。',
    background: '大暑正值"三伏天"里的"中伏"前后，是一年中最热的时期。民间有饮伏茶、晒伏姜、烧伏香等习俗。大暑湿热交蒸，养生重在防暑降温、祛湿健脾。',
    dietary: [
      { food: '烧仙草', reason: '清热解毒，消暑降温，大暑传统饮品' },
      { food: '冬瓜', reason: '清热利水，消肿解毒，夏季养生必备' },
      { food: '羊肉', reason: '大暑吃伏羊，以热制热，排出体内寒气' },
      { food: '荷叶粥', reason: '清热解暑，升发清阳，健脾祛湿' }
    ],
    climate: '一年中最热，高温潮湿，旱涝交替',
    custom: '饮伏茶、晒伏姜、烧伏香、送大暑船',
    poems: '赤日几时过，清风无处寻。——宋·曾几《大暑》'
  },
  {
    name: '立秋', pinyin: 'Lì Qiū', english: 'Start of Autumn',
    season: '秋', month: 8, day: 7,
    description: '立秋标志着秋季的开始，但"秋后一伏"依然炎热。立秋后，阳气渐收，阴气渐长，万物开始从繁茂生长趋向成熟。',
    background: '立秋是重要的节气，古代帝王要举行"迎秋"仪式。民间有"贴秋膘"的习俗，因为夏天炎热食欲不振，到了立秋要多吃肉补充营养。还有"啃秋"吃西瓜的习俗。',
    dietary: [
      { food: '烤肉', reason: '"贴秋膘"，立秋吃烤肉补充夏日消耗' },
      { food: '西瓜', reason: '"啃秋"吃西瓜，防秋燥、消暑气' },
      { food: '茄子', reason: '清热凉血，立秋时令蔬菜' },
      { food: '南瓜', reason: '富含维生素A，保护呼吸道黏膜' }
    ],
    climate: '暑去凉来，但暑气未消，秋老虎可能出现',
    custom: '贴秋膘、啃秋、晒秋、祭土地神',
    poems: '空山新雨后，天气晚来秋。——唐·王维《山居秋暝》'
  },
  {
    name: '处暑', pinyin: 'Chǔ Shǔ', english: 'End of Heat',
    season: '秋', month: 8, day: 23,
    description: '"处"是终止的意思，处暑表示炎热的暑天结束。此时，三伏已过或近尾声，白天热，早晚凉，昼夜温差增大，秋意渐浓。',
    background: '处暑过后，秋意渐浓，是畅游郊野、迎秋赏景的好时节。民间有处暑吃鸭子的习俗，因鸭肉性凉，可防秋燥。还有"处暑开渔节"，处暑过后是渔业收获的时节。',
    dietary: [
      { food: '鸭肉', reason: '处暑吃鸭，滋阴养胃，利水消肿' },
      { food: '百合', reason: '润肺止咳，清心安神，秋季养生佳品' },
      { food: '银耳', reason: '滋阴润燥，美容养颜，适合秋季食用' },
      { food: '梨', reason: '生津止渴，润肺清热，缓解秋燥' }
    ],
    climate: '暑气渐消，秋高气爽，昼夜温差大',
    custom: '吃鸭子、放河灯、开渔节、祭祖',
    poems: '离离暑云散，袅袅凉风起。——唐·白居易《早秋曲江感怀》'
  },
  {
    name: '白露', pinyin: 'Bái Lù', english: 'White Dew',
    season: '秋', month: 9, day: 8,
    description: '白露时节，天气转凉，地面水汽凝结为露。古人以四时配五行，秋属金，金色白，故以白形容秋露，故名"白露"。此时的露水清透晶莹。',
    background: '白露是反映自然界寒气增长的重要节气。有"白露秋风夜，一夜凉一夜"之说。民间有"收清露"的习俗，古人认为露水有神奇功效。白露茶也深受喜爱。',
    dietary: [
      { food: '龙眼', reason: '白露吃龙眼，益气补脾，养血安神' },
      { food: '白露茶', reason: '白露时节采摘的茶叶，醇厚清香' },
      { food: '红薯', reason: '补中和血，益气生津，白露时令美食' },
      { food: '莲藕', reason: '润肺止咳，清心安神，秋季滋补佳品' }
    ],
    climate: '天气转凉，露水出现，秋意渐浓',
    custom: '收清露、饮白露茶、吃龙眼、祭禹王',
    poems: '蒹葭苍苍，白露为霜。——《诗经·秦风·蒹葭》'
  },
  {
    name: '秋分', pinyin: 'Qiū Fēn', english: 'Autumn Equinox',
    season: '秋', month: 9, day: 23,
    description: '秋分这天太阳直射赤道，全球各地昼夜等长。秋分平分了秋季，也平分了昼夜。秋分过后，北半球昼短夜长，天气越来越凉。',
    background: '秋分在古代是"祭月节"，后来因秋分每年的日期不同，不一定都有圆月，将"祭月节"调至中秋。民间有"秋分吃秋菜"、放风筝、送秋牛等习俗。',
    dietary: [
      { food: '秋菜', reason: '"秋分吃秋菜"，觅秋菜煮汤，洗涤肝肠' },
      { food: '螃蟹', reason: '"秋分食蟹忙"，蟹肥菊黄时节' },
      { food: '桂花糕', reason: '桂花香浓，制成糕点，润秋燥' },
      { food: '柿子', reason: '秋分时节柿子红，润肺生津止渴' }
    ],
    climate: '昼夜等长，秋高气爽，天气转凉',
    custom: '祭月、吃秋菜、放风筝、送秋牛',
    poems: '秋分一夜停，阴魄最晶荧。——唐·李频《中秋对月》'
  },
  {
    name: '寒露', pinyin: 'Hán Lù', english: 'Cold Dew',
    season: '秋', month: 10, day: 8,
    description: '寒露是天气由凉爽转向寒冷的过渡。此时，露水更多，气温更低，地面的露水快要凝结成霜了。寒露过后，北方呈现深秋景象，南方秋意渐浓。',
    background: '寒露节气，秋收秋种进入高潮。民间有"寒露登高"的习俗，此时菊花盛开，正是赏菊的好时节。寒露时节也是吃母蟹的最佳时期，蟹黄丰满。',
    dietary: [
      { food: '芝麻', reason: '寒露吃芝麻，润燥养颜，滋养肝肾' },
      { food: '大闸蟹', reason: '"寒露发脚"，螃蟹膏肥黄满，味道鲜美' },
      { food: '山楂', reason: '消食健胃，活血化瘀，应季水果' },
      { food: '菊花茶', reason: '清热明目，解毒消炎，寒露时节饮菊花' }
    ],
    climate: '气温下降明显，露水将凝，秋意萧瑟',
    custom: '登高赏菊、饮菊花酒、吃螃蟹',
    poems: '寒露惊秋晚，朝看菊渐黄。——唐·元稹《寒露九月节》'
  },
  {
    name: '霜降', pinyin: 'Shuāng Jiàng', english: 'Frost Descent',
    season: '秋', month: 10, day: 24,
    description: '霜降是秋季最后一个节气，天气渐冷，开始有霜。霜降不是"降霜"，而是表示天气寒冷，大地将产生初霜的现象。此时，枫叶变红，银杏转黄。',
    background: '霜降节气有"霜降杀百草"之说，农作物停止生长。民间有"霜降吃柿子"的习俗，据说霜降吃柿子不会感冒。还有赏枫叶、赏菊花的传统。',
    dietary: [
      { food: '柿子', reason: '"霜降吃丁柿，不会流鼻涕"，润肺化痰' },
      { food: '栗子', reason: '补肾强筋，活血止血，秋季滋补佳品' },
      { food: '白萝卜', reason: '"十月萝卜小人参"，消食化痰' },
      { food: '牛肉', reason: '霜降过后是进补好时节，牛肉补中益气' }
    ],
    climate: '天气寒冷，开始降霜，秋风萧瑟',
    custom: '吃柿子、赏红叶、赏菊花、送芋鬼',
    poems: '停车坐爱枫林晚，霜叶红于二月花。——唐·杜牧《山行》'
  },
  {
    name: '立冬', pinyin: 'Lì Dōng', english: 'Start of Winter',
    season: '冬', month: 11, day: 7,
    description: '立冬是冬季的开始。"冬"是"终"的意思，万物收藏。此时，水始冰，地始冻。农作物已收割完毕，万物进入休养生息的状态。',
    background: '立冬在古代社会是重要节日，帝王要举行迎冬仪式。民间有"立冬补冬"的习俗，认为冬季是进补的最佳时机。南方有吃甘蔗的习俗，北方有吃饺子的习惯。',
    dietary: [
      { food: '饺子', reason: '立冬吃饺子，寓意"交子之时"，北方传统' },
      { food: '羊肉汤', reason: '温补阳气，驱寒暖身，立冬进补上选' },
      { food: '甘蔗', reason: '立冬食蔗，清热生津，保护牙齿' },
      { food: '核桃', reason: '补肾固精，温肺定喘，冬季进补佳品' }
    ],
    climate: '水始冰，地始冻，万物收藏',
    custom: '迎冬、补冬、吃饺子、祭祖',
    poems: '冻笔新诗懒写，寒炉美酒时温。——唐·李白《立冬》'
  },
  {
    name: '小雪', pinyin: 'Xiǎo Xuě', english: 'Minor Snow',
    season: '冬', month: 11, day: 22,
    description: '小雪节气意味着天气寒冷，降水形式由雨变雪，但雪量还不大，故称"小雪"。小雪节气是寒潮和强冷空气活动频数较高的节气。',
    background: '小雪时节，北方地区开始降雪，大地逐渐冰封。民间有"冬腊风腌，蓄以御冬"的习俗，此时是腌制腊肉、香肠的好时节。还有吃糍粑的传统。',
    dietary: [
      { food: '糍粑', reason: '小雪吃糍粑，甜蜜暖身，南方传统美食' },
      { food: '腊肉', reason: '小雪腌腊肉，香气浓郁，冬季风味' },
      { food: '红薯', reason: '补虚乏、益气力，冬季暖身美食' },
      { food: '白萝卜', reason: '消食化痰，清热生津，冬季养生菜' }
    ],
    climate: '气温骤降，开始降雪但雪量不大',
    custom: '腌腊肉、打糍粑、晒鱼干、吃刨汤',
    poems: '小雪才过大雪前，萧萧风雨纸窗穿。——宋·梅尧臣《小雪》'
  },
  {
    name: '大雪', pinyin: 'Dà Xuě', english: 'Major Snow',
    season: '冬', month: 12, day: 7,
    description: '大雪节气意味着天气更冷，降雪的可能性比小雪时更大了。此时，北方已是"千里冰封，万里雪飘"的严冬景象。',
    background: '大雪是"进补"的好时节，素有"冬天进补，开春打虎"的说法。此时要注意保暖，尤其是头部和脚部。大雪时节，北方有赏雪滑冰的习俗。',
    dietary: [
      { food: '红枣', reason: '大雪吃红枣，补血养气，温暖一冬' },
      { food: '羊肉', reason: '温胃御寒，大雪进补首选' },
      { food: '甘蔗马蹄水', reason: '清热生津，滋阴润燥' },
      { food: '枸杞', reason: '滋补肝肾，益精明目，冬季养生必备' }
    ],
    climate: '气温显著下降，降雪量增大',
    custom: '赏雪、滑冰、进补、腌肉',
    poems: '大雪满弓刀，单于夜遁逃。——唐·卢纶《塞下曲》'
  },
  {
    name: '冬至', pinyin: 'Dōng Zhì', english: 'Winter Solstice',
    season: '冬', month: 12, day: 22,
    description: '冬至是北半球一年中白昼最短的一天。"冬至一阳生"，从这天起，阳气开始回升，白昼一天比一天长。冬至在中国传统文化中地位极高，素有"冬至大如年"的说法。',
    background: '冬至是二十四节气中最重要的节气之一，在周代曾是新年元旦。汉代以冬至为"冬节"，官府要举行祝贺仪式。民间有"数九"的习俗，从冬至开始数九九八十一天，寒冬就过去了。',
    dietary: [
      { food: '饺子', reason: '"冬至不端饺子碗，冻掉耳朵没人管"' },
      { food: '汤圆', reason: '南方冬至吃汤圆，象征团圆美满' },
      { food: '羊肉汤', reason: '温补阳气，冬至养生第一汤' },
      { food: '馄饨', reason: '寓意混沌初开，天地始分，冬至传统美食' }
    ],
    climate: '白昼最短，进入数九寒天，一年最冷开始',
    custom: '祭祖、数九、吃饺子汤圆、赠送鞋袜',
    poems: '天时人事日相催，冬至阳生春又来。——唐·杜甫《小至》'
  },
  {
    name: '小寒', pinyin: 'Xiǎo Hán', english: 'Minor Cold',
    season: '冬', month: 1, day: 6,
    description: '小寒标志着季冬时节的正式开始。小寒是气温最低的节气，只有少数年份的大寒气温低于小寒。小寒时节，土壤冻结，河流封冻，北国大地一片宁静。',
    background: '小寒正值"三九"前后，一年中最寒冷的时期。民间有"小寒大寒，滴水成冰"之说。此时腊梅含苞待放，人们开始准备年货，迎接春节的到来。',
    dietary: [
      { food: '腊八粥', reason: '小寒临近腊八，喝腊八粥暖身祈福' },
      { food: '糯米饭', reason: '小寒吃糯米饭，补中益气暖胃' },
      { food: '鸡汤', reason: '温补暖身，滋补养阴，抵御严寒' },
      { food: '核桃', reason: '补肾温肺，润肠通便，冬季健脑佳品' }
    ],
    climate: '天寒地冻，冷到极致，但阳气已动',
    custom: '数九、探梅、冰戏、吃菜饭',
    poems: '小寒连大吕，欢鹊垒新巢。——唐·元稹《小寒十二月节》'
  },
  {
    name: '大寒', pinyin: 'Dà Hán', english: 'Major Cold',
    season: '冬', month: 1, day: 20,
    description: '大寒是二十四节气中的最后一个节气。大寒过后，又将迎来新一年的节气轮回。此时天气寒冷到极点，但也是冬去春来的转折点。',
    background: '大寒是一年中最冷的时候，也是冬藏的终点。大寒过后就是立春，人们忙着除旧布新、准备年货，迎接新春的到来。民间有"大寒迎年"的习俗。',
    dietary: [
      { food: '年糕', reason: '大寒吃年糕，寓意"年年高升"' },
      { food: '八宝饭', reason: '补气养血，温暖一冬，大寒传统美食' },
      { food: '姜茶', reason: '驱寒暖身，温胃散寒，大寒必备饮品' },
      { food: '桂圆', reason: '补血养心安神，适合寒冬进补' }
    ],
    climate: '一年中最冷，但春天即将到来，阳气渐生',
    custom: '迎年、除旧布新、制作年肴、扫尘',
    poems: '大寒雪未消，闭户不能出。——宋·陆游《大寒》'
  }
];

const GAN_ZHI_60 = (function() {
  const result = [];
  for (let i = 0; i < 60; i++) {
    const tg = TIAN_GAN[i % 10];
    const dz = DI_ZHI[i % 12];
    result.push({
      index: i + 1,
      tianGan: tg,
      diZhi: dz,
      name: tg + dz,
      wuXing: TIAN_GAN_WUXING[i % 10],
      yinYang: TIAN_GAN_YINYANG[i % 10],
      shengXiao: DI_ZHI_SHENGXIAO[i % 12]
    });
  }
  return result;
})();

const QUIZ_BANK = {
  termToSeason: (function() {
    const qs = [];
    SOLAR_TERMS.forEach(t => {
      const wrong = SI_JI.filter(s => s.name !== t.season).map(s => s.name);
      qs.push({
        type: 'termToSeason',
        question: '【' + t.name + '】属于哪个季节？',
        answer: t.season,
        options: shuffleArray([t.season].concat(wrong.slice(0, 3))),
        hint: t.description.substring(0, 20) + '...',
        explain: t.climate
      });
    });
    return qs;
  })(),

  seasonToTerm: (function() {
    const qs = [];
    SI_JI.forEach(s => {
      s.solarTerms.forEach(st => {
        const wrong = SOLAR_TERMS.filter(t => t.season !== s.name).map(t => t.name);
        const shuffledWrong = shuffleArray(wrong).slice(0, 3);
        qs.push({
          type: 'seasonToTerm',
          question: '以下哪个节气属于【' + s.name + '季】？',
          answer: st,
          options: shuffleArray([st].concat(shuffledWrong)),
          hint: s.description.substring(0, 20) + '...',
          explain: st + '：' + (SOLAR_TERMS.find(t => t.name === st) || {}).climate || ''
        });
      });
    });
    return qs;
  })(),

  shichenTime: (function() {
    const qs = [];
    SHI_CHEN.forEach(sc => {
      const wrong = SHI_CHEN.filter(s => s.name !== sc.name);
      const shuffledWrong = shuffleArray(wrong).slice(0, 3);
      qs.push({
        type: 'shichenTime',
        question: '【' + sc.range + '】对应什么时辰？',
        answer: sc.name,
        options: shuffleArray([sc.name].concat(shuffledWrong.map(s => s.name))),
        hint: '地支：' + sc.diZhi + '，属相：' + sc.animal,
        explain: sc.description
      });
      qs.push({
        type: 'shichenDesc',
        question: '【' + sc.name + '】对应什么时间段？',
        answer: sc.range,
        options: shuffleArray([sc.range].concat(shuffledWrong.map(s => s.range).slice(0, 3))),
        hint: '地支：' + sc.diZhi + '，属相：' + sc.animal,
        explain: sc.description
      });
    });
    return qs;
  })(),

  ganzhiMatch: (function() {
    const qs = [];
    GAN_ZHI_60.forEach(gz => {
      const wrong = GAN_ZHI_60.filter(g => g.name !== gz.name);
      const shuffledWrong = shuffleArray(wrong).slice(0, 3);
      qs.push({
        type: 'ganzhiMatch',
        question: '【' + gz.name + '】的生肖是什么？',
        answer: gz.shengXiao,
        options: shuffleArray([gz.shengXiao].concat(shuffledWrong.map(g => g.shengXiao).filter((v, i, a) => a.indexOf(v) === i).slice(0, 3))),
        hint: '天干：' + gz.tianGan + '（' + gz.wuXing + '·' + gz.yinYang + '）',
        explain: gz.name + '：地支' + gz.diZhi + '对应生肖' + gz.shengXiao
      });
      qs.push({
        type: 'ganzhiWuxing',
        question: '【' + gz.name + '】的五行属性是什么？',
        answer: gz.wuXing,
        options: shuffleArray([gz.wuXing].concat(['金', '木', '水', '火', '土'].filter(w => w !== gz.wuXing).slice(0, 3))),
        hint: '生肖：' + gz.shengXiao + '，阴阳：' + gz.yinYang,
        explain: '天干' + gz.tianGan + '属' + gz.wuXing
      });
    });
    return qs;
  })(),

  termMonth: (function() {
    const qs = [];
    SOLAR_TERMS.forEach(t => {
      const wrong = SOLAR_TERMS.filter(s => s.name !== t.name);
      const shuffledWrong = shuffleArray(wrong).slice(0, 3);
      qs.push({
        type: 'termMonth',
        question: '【' + t.name + '】出现在几月？',
        answer: t.month + '月',
        options: shuffleArray([t.month + '月'].concat(shuffledWrong.map(s => s.month + '月').filter((v, i, a) => a.indexOf(v) === i).slice(0, 3))),
        hint: t.season + '季 · ' + t.climate,
        explain: t.name + '：每年' + t.month + '月' + t.day + '日前后'
      });
    });
    return qs;
  })(),

  termDesc: (function() {
    const qs = [];
    SOLAR_TERMS.forEach(t => {
      const wrong = SOLAR_TERMS.filter(s => s.name !== t.name);
      const shuffledWrong = shuffleArray(wrong).slice(0, 3);
      qs.push({
        type: 'termDesc',
        question: '以下哪个节气有这样的描述："' + t.description.split('。')[0] + '"?',
        answer: t.name,
        options: shuffleArray([t.name].concat(shuffledWrong.map(s => s.name).slice(0, 3))),
        hint: t.season + '季 · ' + t.month + '月' + t.day + '日',
        explain: t.climate
      });
    });
    return qs;
  })()
};

function shuffleArray(arr) {
  const a = arr.slice();
  for (let i = a.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [a[i], a[j]] = [a[j], a[i]];
  }
  return a;
}

function generateQuiz(type, count) {
  const bank = QUIZ_BANK[type];
  if (!bank) return [];
  const shuffled = shuffleArray(bank);
  return shuffled.slice(0, Math.min(count || 10, shuffled.length));
}

function getGanZhiByIndex(idx) {
  const i = ((idx % 60) + 60) % 60;
  return GAN_ZHI_60[i === 0 ? 59 : i - 1];
}

function getGanZhiByName(name) {
  return GAN_ZHI_60.find(g => g.name === name) || null;
}

const FESTIVALS = [
  {
    name: '春节', english: 'Spring Festival',
    lunarDate: '正月初一',
    solarDates: { 2024: '2月10日', 2025: '1月29日', 2026: '2月17日', 2027: '2月6日' },
    description: '春节是中华民族最重要的传统节日，承载着丰富的文化内涵和民族感情。它标志着农历新年的开始，也是全家团圆、共度佳节的时刻。',
    background: '春节源于殷商时期年头岁尾的祭神祭祖活动，距今已有4000多年历史。传说古代有一种叫"年"的怪兽，每到除夕出来害人，人们发现它害怕红色、火光和炸响，于是形成了贴春联、放鞭炮等习俗。',
    customs: ['贴春联', '放鞭炮', '吃年夜饭', '拜年', '发红包', '守岁', '舞龙舞狮'],
    foods: ['饺子', '年糕', '鱼（年年有余）', '汤圆', '春卷'],
    poems: '爆竹声中一岁除，春风送暖入屠苏。——宋·王安石《元日》'
  },
  {
    name: '元宵节', english: 'Lantern Festival',
    lunarDate: '正月十五',
    solarDates: { 2024: '2月24日', 2025: '2月12日', 2026: '3月3日', 2027: '2月20日' },
    description: '元宵节又称上元节、灯节，是春节过后的第一个重要节日。这一天人们赏花灯、猜灯谜、吃元宵，热闹非凡。',
    background: '元宵节起源于汉代，汉明帝为了弘扬佛法，下令正月十五夜在宫中和寺院"燃灯表佛"。此后元宵放灯的习俗流传到民间。道教将正月十五定为"上元节"。',
    customs: ['赏花灯', '猜灯谜', '吃元宵/汤圆', '舞龙灯', '踩高跷', '走百病'],
    foods: ['元宵', '汤圆'],
    poems: '东风夜放花千树，更吹落，星如雨。——宋·辛弃疾《青玉案·元夕》'
  },
  {
    name: '龙抬头', english: 'Dragon Head Festival',
    lunarDate: '二月初二',
    solarDates: { 2024: '3月11日', 2025: '3月1日', 2026: '3月20日', 2027: '3月9日' },
    description: '二月二龙抬头，又称春耕节、农事节。传说这一天龙王抬头，开始行云布雨，所以叫"龙抬头"。人们在这一天理发，寓意"剃龙头"，祈求好运。',
    background: '龙抬头源于古代的天象崇拜，东方苍龙七宿在农历二月初二开始从地平线升起，宛如巨龙抬头。民间认为这一天是龙王布雨的开始，对春耕至关重要。',
    customs: ['剃龙头', '吃龙食', '祭土地神', '引钱龙', '开笔礼'],
    foods: ['春饼（吃龙鳞）', '面条（吃龙须）', '饺子（吃龙耳）', '猪头肉'],
    poems: '二月二日新雨晴，草芽菜甲一时生。——唐·白居易《二月二日》'
  },
  {
    name: '端午节', english: 'Dragon Boat Festival',
    lunarDate: '五月初五',
    solarDates: { 2024: '6月10日', 2025: '5月31日', 2026: '6月19日', 2027: '6月8日' },
    description: '端午节是中国四大传统节日之一，又称端阳节、龙舟节，是纪念伟大诗人屈原的节日。也是夏季驱除瘟疫的节日。',
    background: '端午节已有2500多年历史。传说战国时期楚国诗人屈原在五月五日跳汨罗江自尽，百姓们奋力划船追赶拯救，后来演变为赛龙舟。人们投入粽子以防鱼虾损伤屈原身体。',
    customs: ['赛龙舟', '吃粽子', '挂艾草菖蒲', '佩香囊', '饮雄黄酒', '系五色线'],
    foods: ['粽子', '咸鸭蛋', '绿豆糕', '黄酒'],
    poems: '节分端午自谁言，万古传闻为屈原。——唐·文秀《端午》'
  },
  {
    name: '七夕节', english: 'Qixi Festival',
    lunarDate: '七月初七',
    solarDates: { 2024: '8月10日', 2025: '8月29日', 2026: '8月17日', 2027: '8月7日' },
    description: '七夕节又称乞巧节，是中国最具浪漫色彩的传统节日。传说牛郎织女在鹊桥相会，被视为"中国情人节"。',
    background: '七夕节起源于汉代，牛郎织女的爱情传说流传千年。织女是天帝的孙女，擅长织布，与牛郎相爱后结为夫妇。后被天帝分开，只允许每年七月初七鹊桥相会。',
    customs: ['乞巧', '拜织女', '穿针引线', '晒书晒衣', '做巧果', '看牵牛织女星'],
    foods: ['巧果', '巧酥', '瓜果', '花瓜'],
    poems: '纤云弄巧，飞星传恨，银汉迢迢暗度。——宋·秦观《鹊桥仙》'
  },
  {
    name: '中元节', english: 'Ghost Festival',
    lunarDate: '七月十五',
    solarDates: { 2024: '8月18日', 2025: '9月6日', 2026: '8月24日', 2027: '8月14日' },
    description: '中元节又称鬼节、盂兰盆节。民间有祭祖、放河灯等习俗，是为逝者祈福超度的重要节日。',
    background: '中元节源于道教"三元说"（上元正月十五、中元七月十五、下元十月十五）。佛教传入中国后，"盂兰盆会"与中元节融合。传说农历七月鬼门打开，人们通过祭祀表达对先人的思念。',
    customs: ['祭祖', '放河灯', '烧纸钱', '做法事', '吃鸭（压惊）'],
    foods: ['鸭肉', '面人', '扁食', '濑粉'],
    poems: '江南水寺中元夜，金粟栏边见月娥。——唐·李郢《中元夜》'
  },
  {
    name: '中秋节', english: 'Mid-Autumn Festival',
    lunarDate: '八月十五',
    solarDates: { 2024: '9月17日', 2025: '10月6日', 2026: '9月25日', 2027: '9月14日' },
    description: '中秋节是中国最重要的传统节日之一，象征团圆。中秋之夜，明月当空，人们赏月、吃月饼，寄托思念故乡、思念亲人之情。',
    background: '中秋节起源于古代对月的崇拜，由上古时代秋夕祭月演变而来。传说后羿射日得仙药，其妻嫦娥误食而奔月。后羿面对明月思念妻子，人们因此有了中秋赏月、祭月的传统。',
    customs: ['赏月', '吃月饼', '祭月', '观潮', '燃灯', '玩花灯', '饮桂花酒'],
    foods: ['月饼', '桂花糕', '芋头', '田螺', '柚子'],
    poems: '但愿人长久，千里共婵娟。——宋·苏轼《水调歌头》'
  },
  {
    name: '重阳节', english: 'Double Ninth Festival',
    lunarDate: '九月初九',
    solarDates: { 2024: '10月11日', 2025: '10月29日', 2026: '10月18日', 2027: '10月7日' },
    description: '重阳节又称重九节、老人节，是敬老爱老的传统节日。因"九"为阳数之极，九月九日两九相重，故称重阳。',
    background: '重阳节起源于战国时期，汉代正式成为节日。相传东汉时期，汝南人桓景经仙人指点，在九月九日携全家登高避祸，躲过了一场瘟疫，此后登高成为重阳重要习俗。',
    customs: ['登高', '赏菊', '插茱萸', '饮菊花酒', '吃重阳糕', '敬老'],
    foods: ['重阳糕', '菊花酒', '菊花茶', '柿子', '螃蟹'],
    poems: '独在异乡为异客，每逢佳节倍思亲。——唐·王维《九月九日忆山东兄弟》'
  },
  {
    name: '腊八节', english: 'Laba Festival',
    lunarDate: '腊月初八',
    solarDates: { 2024: '1月18日', 2025: '1月7日', 2026: '1月24日', 2027: '1月14日' },
    description: '腊八节是腊月的重要节日，人们喝腊八粥，祭祀祖先和神灵，祈求丰收和吉祥。腊八节的到来，意味着春节的脚步越来越近。',
    background: '腊八节源于古代的腊祭，是年终祭祀天地祖先的仪式。佛教传入后，传说释迦牟尼在腊月初八成道，寺院会煮粥供佛，并分施给民众，形成了喝腊八粥的习俗。',
    customs: ['喝腊八粥', '泡腊八蒜', '祭祖', '制腊八豆腐', '吃腊八面'],
    foods: ['腊八粥', '腊八蒜', '腊八面', '腊八豆腐'],
    poems: '腊日常年暖尚遥，今年腊日冻全消。——唐·杜甫《腊日》'
  },
  {
    name: '除夕', english: 'Chinese New Year Eve',
    lunarDate: '腊月三十（或二十九）',
    solarDates: { 2024: '2月9日', 2025: '1月28日', 2026: '2月16日', 2027: '2月5日' },
    description: '除夕是农历年的最后一天，又称大年夜。"除"是去除之意，夕指夜晚。除夕是一家人团聚守岁、辞旧迎新的重要时刻。',
    background: '除夕起源于古代的"逐除"仪式，人们击鼓驱逐"疫疠之鬼"。传说"年兽"在除夕出来伤人，人们发现用红色、火光和鞭炮声可以驱赶它，形成了贴春联、放鞭炮等习俗。',
    customs: ['贴春联', '贴福字', '放鞭炮', '吃年夜饭', '守岁', '给压岁钱'],
    foods: ['年夜饭', '鱼（年年有余）', '饺子', '年糕', '八宝饭'],
    poems: '千门万户曈曈日，总把新桃换旧符。——宋·王安石《元日》'
  }
];

function getYearTianGanDiZhi(year) {
  const tgIndex = (year - 4) % 10;
  const dzIndex = (year - 4) % 12;
  return {
    tianGan: TIAN_GAN[tgIndex],
    diZhi: DI_ZHI[dzIndex],
    wuXing: TIAN_GAN_WUXING[tgIndex],
    shengXiao: DI_ZHI_SHENGXIAO[dzIndex],
    yinYang: TIAN_GAN_YINYANG[tgIndex],
    fullName: TIAN_GAN[tgIndex] + DI_ZHI[dzIndex] + '年'
  };
}

function getMonthTianGanDiZhi(year, month) {
  const yearTgIndex = (year - 4) % 10;
  const monthTgIndex = (yearTgIndex * 2 + month - 1) % 10;
  const monthDzIndex = (month + 2) % 12;
  return {
    tianGan: TIAN_GAN[monthTgIndex],
    diZhi: DI_ZHI[monthDzIndex],
    fullName: TIAN_GAN[monthTgIndex] + DI_ZHI[monthDzIndex] + '月'
  };
}

function getCurrentShiChen(date) {
  if (!date) date = new Date();
  const hours = date.getHours();
  let index;
  if (hours >= 23 || hours < 1) index = 0;
  else if (hours < 3) index = 1;
  else if (hours < 5) index = 2;
  else if (hours < 7) index = 3;
  else if (hours < 9) index = 4;
  else if (hours < 11) index = 5;
  else if (hours < 13) index = 6;
  else if (hours < 15) index = 7;
  else if (hours < 17) index = 8;
  else if (hours < 19) index = 9;
  else if (hours < 21) index = 10;
  else index = 11;
  return SHI_CHEN[index];
}

function getSolarTermForDate(month, day, year) {
  const terms = SOLAR_TERMS.filter(t => {
    return t.month === month && Math.abs(t.day - day) <= 2;
  });
  if (terms.length === 1) return terms[0];
  if (terms.length > 1) {
    return terms.reduce((prev, curr) => Math.abs(curr.day - day) < Math.abs(prev.day - day) ? curr : prev);
  }
  return null;
}

function getSolarTermsForMonth(month) {
  return SOLAR_TERMS.filter(t => t.month === month);
}

function getSeasonForMonth(month) {
  for (let season of SI_JI) {
    if (season.months.includes(month)) return season;
  }
  return SI_JI[3];
}

function getFestivalsForDate(year, month, day) {
  const key = year;
  const dateStr = `${month}月${day}日`;
  return FESTIVALS.filter(f => {
    if (f.solarDates[key] === dateStr) return true;
    return false;
  });
}

function getSolarTermByName(name) {
  return SOLAR_TERMS.find(t => t.name === name);
}

function getSeasonByName(name) {
  return SI_JI.find(s => s.name === name);
}
