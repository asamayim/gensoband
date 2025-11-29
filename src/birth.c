/* Purpose: create a player character */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/*
 * How often the autoroller will update the display and pause
 * to check for user interuptions.
 * Bigger values will make the autoroller faster, but slower
 * system may have problems because the user can't stop the
 * autoroller for this number of rolls.
 */
#define AUTOROLLER_STEP 5431L

/*
 * Define this to cut down processor use while autorolling
 */
#if 0
#  define AUTOROLLER_DELAY
#endif

/*
 * Maximum number of tries for selection of a proper quest monster
 */
#define MAX_TRIES 100

//#define MAX_CLASS_CHOICE     MAX_CLASS
///mod140202 ユニーククラス実装のために通常選択職業番号最大を別にした
//v1.1.87 カード売人追加のため27→28 これ以上一般職を増やすならこの辺のルーチンの書き直しが必要。
//v2.0.19 養蜂家実装により職業選択部分を変更。この定数を廃止
//#define MAX_CLASS_CHOICE     28


static bool old_data_cheat_flag = FALSE;
static bool old_collector = FALSE;

/*
 * Forward declare
 */
typedef struct hist_type hist_type;

/*
 * Player background information
 */
struct hist_type
{
	cptr info;			    /* Textual History */

	byte roll;			    /* Frequency of this entry */
	byte chart;			    /* Chart index */
	byte next;			    /* Next chart index */
	byte bonus;			    /* Social Class Bonus + 50 */
};

///del1401 ここにbg[]として生い立ちのテンプレ文章があったが全部消した




static cptr info_difficulty[5] ={
	"練習用のモードです。プレイヤーの能力が上がり、ゲームオーバーになってもコンティニューが可能です。このモードは50階で終了します。スコアが大幅に低下します。",
	"通常のモードです。敵の体力が低く、またあまりひどいことは起こりにくいように調整されています。50階クリア時点でゲームクリア扱いになります。",
	"敵がやや力強くなり、しばしば序盤から強敵が出現し、いくつかの事柄に不規則性が出ます。基本的に変愚蛮怒と同様の設定ですが、強いキャラクターでプレイすれば変愚蛮怒より簡単です。スコアはNORMALの2倍です。HARD以降でないと選択できない職業や性格が存在します。",
	"敵が凄まじく力強く素早く注意深くなり、有り得ないほどの強敵が頻繁に出現し、理不尽なまでの様々な困難があなたに襲い掛かります。スコアはNORMALの8倍です。",
	"極めて特殊なモードです。鉄獄1階からゲームが開始されダンジョンから出たり階段を上ることはできません。レベルが上がりやすくなり、深層のアイテムが出現しやすくなり、消耗品が複数まとめて生成され、「アイテムカード」が出現します。ランダムクエストはなくなり、一定階ごとに特定のクエストダンジョンが生成されます。十全な準備を整えられない中で最善を尽くすことが求められます。スコアはNORMALの4倍です。"
};

static cptr race_jouhou[MAX_RACES] =
{
#ifdef JP
"あなたは普通の人間です。人間は主に人里に住んでいますが、外界から迷い込んでくることもあります。幻想郷のほかの住人達に比べると寿命が短く肉体的にも貧弱ですが、成長の速さには特筆すべきものがあります。秀でた点がない代わりに弱点らしい弱点もなく、どのような職業でもそれなりにこなすことが可能です。",
  
"あなたは幻想郷のあちこちに棲み着く若き妖怪たちの一人です。若いと言っても軽く百年は生きているかもしれませんし、ひょっとすると昨日生まれたばかりかもしれません。妖怪は一般に通常の人間を大きく上回る身体能力を持ちますが、小難しいことを考えるのは苦手で術師には向きません。多くは破邪の攻撃に弱く、破邪属性の攻撃や魔法により大きなダメージを受けてしまいます。",
  
"あなたは人間と妖怪のハーフです。通常の人間よりはやや丈夫な体を持ち、レベルが上がると毒に対する耐性を得ます。また比較的少ない食事で活動することが可能です。弱点らしい弱点は持たず、どのような職業でもそれなりにこなせます。人間に比べると長い寿命を持ちますが、そのぶん成長がやや遅くなります。",
  
"あなたは仙人です。長い修行の果てに寿命を克服し、さらなる高みを求めて修行を続けています。身体能力は常人並みですがその精神は研ぎ澄まされており、とくに仙術を扱うことに関しては最高クラスの適性を持ちます。またごくわずかな食糧で活動することが可能です。長く生きて様々なことを経験してきたためやや成長が遅くなります。本来の寿命を無理矢理に伸ばしているためしばしば地獄からの刺客に狙われます。レベルが上がると混乱などに対する耐性を得ます。",
  
"あなたは天界の住人です。日々歌を詠んだり踊ったりと自由気ままに暮らしていますが、通常の人間を遙かに凌駕する戦闘能力を持ちます。仙術に対して最高クラスの適性を持ちますが妖術は全くと言っていいほど扱えません。長い時を生きているので成長速度は緩慢です。仙人と同様にしばしば地獄からの刺客に狙われます。天人の清浄な肉体は破邪の力による攻撃を全く受け付けませんが、天人の肉体に大打撃を与える攻撃も存在します。",
  
"あなたは妖精です。妖精は自然の力の具現で幻想郷の至る所に出現します。悪戯が大好きですが純粋な戦闘行為は絶望的に不得手です。身体能力、知性ともに見るべきところはほとんどありません。その上自らを鍛えるという発想がないため成長速度もやや緩慢です。唯一隠れることだけは得意です。もし冒険を全うできれば大きな誉れとなることでしょう。",
  
"あなたは河童です。水辺に住み泳ぎが達者で絶対に溺れることがありません。妖怪のわりにはそれほど強靭な肉体を持っていませんが、水の中では見違えるような力強さを発揮します。腕を伸ばして遠くの敵に攻撃する独特の特技を持ちます。水の耐性のほか、成長すれば酸への耐性を獲得しますが電撃は苦手です。もし河童がエンジニアになった場合は水を弱点としない強力な機械を扱えます。",
  
"あなたは魔法使いです。ここで言う魔法使いとは職業ではなく、魔法の探求の果てに自らの魔法で自らを活動させるようになった者達のことをいいます。元は人間であった者が多いのですが生まれつきの魔法使いも存在します。身体能力は貧弱ですがその知性と魔法全般への適性は絶大です。レベルが上がるにつれその精神はさらに堅牢さを増し、混乱や知能低下攻撃への耐性を獲得します。また生物に接触して魔力を吸収することができるようになります。魔法使いは魔力で活動しているため一切の食事をしなくても倒れません。",
  
"あなたは鴉天狗です。天狗は妖怪の山で一大勢力を築く集団で、鴉天狗はその中で報道部隊の役割を担っています。極めて高い敏捷性を誇り、知性も比較的高く妖術や自然の仙術を得意としています。特殊なカメラを携えており敵の弾幕攻撃やボルト魔法を消去して無効化することがあります。天狗は頭に「頭襟」を装備しなければなりません。",
  
"あなたは白狼天狗です。天狗は妖怪の山で一大勢力を築く集団で、白狼天狗はその中で自衛団の役割を担っています。高い白兵能力と探知能力を誇り、独力で過酷な環境に適応することができます。天狗は頭に「頭襟」を装備しなければなりません。",
  
"あなたは鬼です。鬼は古くから語り継がれる強力な妖怪で、全種族最高の剛腕と体力を誇り白兵戦ではおそらく最強です。一方で小細工に頼ることを好まず魔法や探索などは苦手です。鬼は立派な角を頭に生やしており、これは格闘時に強力な武器として機能しますが一部の頭防具が装備できなくなるデメリットがあります。かなりの大飯食らいなので食糧は多めに持ちましょう。",
  
"あなたは死神です。死神は彼岸の住人で、三途の川の船頭や閻魔様の裁判の書記など多彩な役職に従事しています。戦闘能力は高く、麻痺や混乱など多くの攻撃に耐性を持ちます。大鎌を装備すると何となく箔が付いて武器の威力が上昇します。",
  
"あなたは獣人です。獣人は人と獣の両方の特徴を持つ存在で、生まれつきの獣人と獣人化した人間と人化した獣に分かれます。多くの獣人は人間より高い身体能力を持ち、その一方で知性は低下しています。満月の晩には力が増します。獣人は獣の鋭敏な感覚を持ち、危険を察することが得意です。",

"あなたは付喪神です。付喪神は古い道具が自我を持った存在で、神の一種ですが実態はほぼ妖怪です。戦闘能力はやや低めですが、道具の心がわかるのか罠解除や魔道具使用が得意です。レベルが上がると呪いのかかったものを装備してもその悪影響を緩和することが可能になります。通常の生物の身体構造を持たないため毒や目潰しなどの影響をあまり受けません。食事の代わりに魔道具のエネルギーを吸収して栄養を得ます。破邪と劣化攻撃が弱点です。",

"あなたは妖怪として自我を得た古い人形で、付喪神の一種です。敵を倒すことでは経験値を得ることができず、身につけた武具の力を自分の力と同化させてレベルアップします。強力な武具が早期に手に入れば一気に冒険が楽になることでしょう。どちらかというと術士より戦士寄りの特性を持ちます。妖怪人形は魔法の杖などから魔力を吸い取って栄養にします。魔力で活動しているため劣化攻撃が弱点です。人形の体のため毒や冷気が効きにくいほか、成長すると恐怖や狂気に対する耐性を得ます。",
  
"あなたは月から来た妖怪兎です。月の防衛隊として戦闘訓練を積んでいたため様々な技能に秀でますが、根が怠け者のため成長はやや遅めです。大きな耳を使ってどこにいても月の噂を聞くことができます。穢れなき月の住人のため破邪攻撃に耐性を持ちますが、ある一部の攻撃に対しては大打撃を受けてしまいます。",
  
"あなたは人化した妖怪狐です。妖怪の割には非力ですが妖怪らしからぬ高い知性を持ち術士に向いています。とくに変身する術を得意としています。職業に関係なく敵を惑わせる能力を持っており、成長すると精神感知能力を獲得します。さらに獣の鋭敏な感覚を備えているため生存能力が高めです。高レベルになると若干の神格を得て破邪を弱点としなくなることがあります。強力な種族ですが成長にはやや長い時間がかかります。",
  
"あなたは山に住まう河童に似た妖怪です。山や森での活動が得意で、森地形の中では普段以上の力を発揮します。また浮遊能力がなくても険しい山に登ることができます。",
  
"あなたは人化した妖怪狸です。狡知に長けており変身によって人を騙したり驚かせることを好みますが、肉弾戦も苦手ではありません。成長すると煙のように姿を消して敵の攻撃を躱す特技を習得します。",
  
"あなたは人魚です。人間の上半身と巨大な魚の下半身を持っています。巨大な尾のため靴を履くことができず、浮遊せずに地上にいる場合移動速度が減少します。しかし水中では極めて素早く行動でき、近接戦闘のときも大きな尾による打撃にはかなりの威力が期待できます。魔法の適性も低くはなく主に妖術が得意です。",
  
"あなたはホフゴブリンです。家に幸福を招くとされ座敷童の後釜として幻想郷に招かれたものの見た目のあまりの怖さに人里から追い出されてしまった気の毒な妖怪です。幼児並みの体格のため肉弾戦はやや苦手ですが、技能や成長性に関しては案外優秀です。成長すると毒と麻痺に対する耐性を得ます。",
  
"あなたは入道です。立派な体格を持ちますが出たり消えたり大きくなったり小さくなったりと掴みどころのない妖怪です。その実体の曖昧さにより物理防御力が高く、成長すると麻痺、破片、轟音攻撃に対し耐性を得ます。戦士向きの能力を持ち魔法の習得は苦手ですが、変容の妖術だけは得意です。",
  
"あなたは小悪魔です。小悪魔はあまり力を持たない悪魔の総称です。知能が高めでやや術士寄りの能力を持ちますが肉弾戦もそれなりにこなせます。魔術と妖術が得意で仙術はかなり苦手です。小悪魔は暗黒と地獄攻撃への耐性を持ち異世界の悍ましい存在を目にしてもあまり悪影響を受けませんが、破邪攻撃により普通の妖怪以上の打撃を受けてしまいます。",
  
"あなたは魔法によって仮の命を吹き込まれた石塊です。高い腕力と耐久力を持ちますが動きは極めて緩慢かつ鈍重です。知性もほとんどなく魔法の習得は極めて困難です。石の体のため多くの攻撃に耐性を持ちますが、劣化(DISENCHANT)の攻撃に対しては大打撃を受けてしまいます。食糧は岩や鉱石類で、レアな鉱石を食べると体力が回復したり有利な効果が出ることがあります。レベルが上がるとダンジョンの壁をそのまま食べることが出来るようになります。",
  
"あなたは小人です。普通の人間の膝下程度の身長しか持たない一族で、体力は妖精や幽霊にも劣り全種族最低です。その上あまりの体の小ささのため重いものを装備できません。しかし隠密能力と回避能力は群を抜いて高く、道を歩くのも命懸けなせいか成長の早さも全種族で一番です。探索や魔道具使用も得意です。",
  
"あなたは大陸の魔法によって操られ腐敗を免れている死体です。体が硬くぎくしゃくした動きしかできませんが時間をかけて鍛えればそれなりに柔軟な動きができるようになります。腕力と耐久力は高く毒や地獄や暗黒など様々な攻撃への耐性を持ちますが破邪攻撃に対し極めて脆弱です。",
  
"あなたは強力な夜の眷族の一員です。人間を遙かに超える身体能力に加え不死身と見紛うほどの耐久力を誇ります。満月の夜になるとその力は更に増しますが、太陽が昇ると弱体化してしまいます。多くの属性に対する抵抗力を持つ一方弱点も多く、閃光と水の攻撃で大ダメージを受け、破邪攻撃ではさらに甚大なダメージを受けてしまいます。肉弾戦と魔法の両方が得意ですが仙術は苦手です。吸血鬼は光源がなくても暗いダンジョンを見通す能力を持っています。通常の食事では空腹を満たすことができず、生物に噛みついて血液を吸い取ることで栄養を得ます。",
  
"あなたは死してなお現世に留まる魂です。壁をすり抜けることができ、ダンジョン探索において非常に有利です。ただし体が希薄なため白兵戦能力と耐久力は最低クラスです。地獄の力に対して完全な免疫を持ち、毒や破片といった危険な攻撃にも耐性を持ちますが、光と破邪の攻撃に対しては非常に脆弱です。レベルアップにより近くの思念を感知する能力を得ます。なお、食事は普通に摂れます。",
  
"あなたは祀られる神社を持たない雑多な神様たちのうちの一柱で、信仰をほぼ失い野山でひっそり暮らしていました。冒険を通して様々な経験を積むことで信仰が集まり、レベルが一定になると新たな神性を得て種族的な特性が大きく変わります。どのように変化するかについては様々な要素に影響されますが、例えば名のある強敵を倒したりクエストをこなして名声を高めることは多くの場合良い結果をもたらすでしょう。なお、あなたが最初何の神様なのかは全くランダムに決まります。",
  
"あなたは長く生きて力を蓄えた大妖怪です。普通の妖怪より明らかに強い力を持ちますが、あなたにとってこの世のすべては見慣れたものばかりなので成長の速度は極めて緩慢です。大妖怪とはいえ依然として破邪攻撃はあなたの弱点です。",
  
"あなたはいくらかの神性を持った人間です。見えないものを見る能力を持ち、通常の人間より全体的な能力がやや高めです。レベルが上がると遅消化と急回復の能力を得ます。成長は人間より少し遅くなり、スコアもやや低下します。",

"あなたはやんごとなき事情により地上に降りてきた貴き月の民です。謀略渦巻く月の都で永い時を過ごしてきたため、その狡知は並の地上人の及ぶところではありません。	あなたは死の匂いと生命の息吹を「穢れ」として強く忌避しています。そのため妖術に分類される魔法があまり使えず生命や自然の魔法も大の苦手です。	あなたは破邪攻撃に対する完全な免疫を持ちますが、穢れた力で攻撃されると大ダメージを受けてしまいます。様々な技能に優れますがあまり肉弾戦を好みません。",
  
"あなたは平安時代のニンジャソウルの憑依によってニンジャとして生まれ変わりました。",
  
"あなたは極めて珍しい人間と幽霊のハーフです。人間の体に寄り添うように人魂がひとつ浮かんでおりどちらもあなたの体です。あなたは通常の人間より死に近いところにいるため死に関係することに対してある程度の適性を持ちます。また気配が薄いため隠密行動が得意です。",
  
"あなたは本来は人間の膝程度の背丈しかない小人ですが、今は小槌の力で人間大になっています。",
  
"あなたは死霊術の奥義により強力なアンデッドに転生しました。魔法に関する能力が飛躍的に上昇し、いくつかの邪悪な特技を習得します。肉弾戦には全く向きませんが、あなたは肉体に頼らず戦う方法を既に充分に知っています。耐久力や耐性も優秀ですが、破邪攻撃を受けると被害は極めて甚大です。",
  
"あなたは究極の生命体です。あらゆる生物の持つ能力を行使できます。",

"あなたは、様々な物事に宿る本質が神話伝承により切り取られ定義づけられた存在です。通常の生物や無生物とは全く異なった在り方をしているため、あらゆる種族変容を受け付けません。",

"あなたは女子高生です。外界で唯一無二の最強無敵の種族です。",

"あなたは禁断の秘薬によって不老不死となった存在です。あらゆる種族変容、突然変異を受け付けません。また変身系の魔法も効果を発揮しません。",

"あなたはかつてお地蔵様でしたが、魔法の力によって生命を得て動き出しました。ゴーレムと魔法使いの中間のような存在で、食事を必要とせず丈夫な体を持ち魔法への適性が高めです。ただし動作はかなり緩慢で注意力にも欠けています。",

"あなたは造形神によって土と水から創り出され、文字通りの神業により魂を吹き込まれた人間サイズの埴輪(ハニワ)です。あなたは魂を持ちながらも霊体を持たない偶像のため、霊的・精神的な攻撃に対しほぼ絶対的な抵抗力を発揮します。また毒や麻痺といった生物を害する攻撃に対しても耐性があります。しかしあなたの体は強い衝撃を受けると割れてしまうかもしれません。破片や轟音の攻撃には注意しましょう。",

"あなたは弱肉強食の畜生界に棲む動物霊です。動物霊の中でも力を持っており、常に人間に近い姿に変化しています。畜生界では多数の巨大組織が覇権を巡って争っており、ほとんどの動物霊はどれかの組織に属しています。属する組織によってレイシャルパワーが変化します。あなたは近接戦闘が得意で様々な攻撃に対する耐性を持ちますが光と破邪の攻撃は大の苦手です。またあなたは通常の食料ではほとんど栄養を得ることができず、敵を倒して力を吸収することで活力を得ます。",

"あなたは人里で人気の座敷わらしです。本来座敷わらしは屋敷から出歩くことができませんが、由緒ある器のような物品に取り憑いて移動式屋敷として持ち歩く手法が発見され、あなたも念願の旅に出られるようになりました。冒険の旅で新たな屋敷になりうる品を発見したら乗り換えることができ、屋敷の特性により能力や特技が多少変化します。あなたは探索・感知系の能力に優れますが、子供そのものの体格なので荒事には向かないかもしれません。",

#else

"The human is the base character.  All other races are compared to them.  Humans can choose any class and are average at everything.  Humans tend to go up levels faster than most other races because of their shorter life spans.  No racial adjustments or intrinsics occur to characters choosing human.",

"Half-elves tend to be smarter and faster than humans, but not as strong.  Half-elves are slightly better at searching, disarming, saving throws, stealth, bows, and magic, but they are not as good at hand weapons.  Half-elves may choose any class and do not receive any intrinsic abilities.",

"Elves are better magicians then humans, but not as good at fighting.  They tend to be smarter and faster than either humans or half-elves and also have better wisdom.  Elves are better at searching, disarming, perception, stealth, bows, and magic, but they are not as good at hand weapons.  They resist light effects intrinsically.",

"Hobbits, or Halflings, are very good at bows, throwing, and have good saving throws.  They also are very good at searching, disarming, perception, and stealth; so they make excellent rogues, but prefer to be called burglars.  They are much weaker than humans, and no good at melee fighting.  Halflings have fair infravision, so they can detect warm creatures at a distance.  They have a strong hold on their life force, and are thus intrinsically resistant to life draining.",

"Gnomes are smaller than dwarves but larger than Halflings.  They, like the hobbits, live in the earth in burrow-like homes.  Gnomes make excellent mages, and have very good saving throws.  They are good at searching, disarming, perception, and stealth.  They have lower strength than humans so they are not very good at fighting with hand weapons.  Gnomes have fair infra-vision, so they can detect warm-blooded creatures at a distance.  Gnomes are intrinsically protected against paralysis.",

"Dwarves are the headstrong miners and fighters of legend.  Dwarves tend to be stronger and tougher but slower and less intelligent than humans.  Because they are so headstrong and are somewhat wise, they resist spells which are cast on them.  They are very good at searching, perception, fighting, and bows.  Dwarves  have a miserable stealth.  They can never be blinded.",

"Half-orcs make excellent warriors, but are terrible at magic.  They are as bad as dwarves at stealth, and horrible at searching, disarming, and perception.  Half-orcs are quite ugly, and tend to pay more for goods in town.  Because of their preference to living underground to on the surface, half-orcs resist darkness attacks.",

"Half-Trolls are incredibly strong, and have more hit points than most other races.  They are also very stupid and slow.  They are bad at searching, disarming, perception, and stealth.  They are so ugly that a Half-Orc grimaces in their presence.  They also happen to be fun to run...  Half-trolls always have their strength sustained.  At higher levels, Half-Trolls regenerate wounds automatically, and if he or her is warrior slowly.",

"The Amberites are a reputedly immortal race, who are endowed with numerous advantages in addition to their longevity.  They are very tough and their constitution cannot be reduced, and their ability to heal wounds far surpasses that of any other race.  Having seen virtually everything, very little is new to them, and they gain levels much slower than the other races.",

"High-elves are a race of immortal beings dating from the beginning of time.  They are masters of all skills, and are strong and intelligent, although their wisdom is sometimes suspect.  High-elves begin their lives able to see the unseen, and resist light effects just like regular elves.  However, there are few things that they have not seen already, and experience is very hard for them to gain.",

"Barbarians are hardy men of the north.  They are fierce in combat, and their wrath is feared throughout the world.  Combat is their life: they feel no fear, and they learn to enter battle frenzy at will even sooner than half-trolls.  Barbarians are, however, suspicious of magic, which makes magic devices fairly hard for them to use. ",

"Half-Ogres are like Half-Orcs, only more so.  They are big, bad, and stupid.  For warriors, they have all the necessary attributes, and they can even become wizards: after all, they are related to Ogre Magi, from whom they have learned the skill of setting trapped runes once their level is high enough.  Like Half-Orcs, they resist darkness, and like Half-Trolls, they have their strength sustained.",

"Half-Giants limited intelligence makes it difficult for them to become full spellcasters, but with their huge strength they make excellent warriors.  Their thick skin makes them resistant to shards, and like Half-Ogres and Half-Trolls, they have their strength sustained.",

"Half-mortal descendants of the mighty titans, these immensely powerful creatures put almost any other race to shame.  They may lack the fascinating special powers of certain other races, but their enhanced attributes more than make up for that.  They learn to estimate the strengths of their foes, and their love for law and order makes them resistant to the effects of Chaos.",

"With but one eye, a Cyclops can see more than many with two eyes.  They are headstrong, and loud noises bother them very little.  They are not quite qualified for the magic using professions, but as a certain Mr.  Ulysses can testify, their accuracy with thrown rocks can be deadly...",

"Yeeks are among the most pathetic creatures.  Fortunately, their horrible screams can scare away less confident foes, and their skin becomes more and more resistant to acid, as they gain experience.  But having said that, even a mediocre monster can wipe the proverbial floor with an unwary Yeek.",

"Klackons are bizarre semi-intelligent ant-like insectoid creatures.  They make great fighters, but their mental abilities are severely limited.  Obedient and well-ordered, they can never be confused.  They are also very nimble, and become faster as they advance levels.  They are also very acidic, inherently resisting acid, and capable of spitting acid at higher levels. ",

"Kobolds are a weak goblin race.  They love poisoned weapons, and can learn to throw poisoned darts (of which they carry an unlimited supply).  They are also inherently resistant to poison, although they are not one of the more powerful races.",

"The hated and persecuted race of nocturnal dwarves, these cave-dwellers are not much bothered by darkness.  Their natural inclination to magical items has made them immune to effects which could drain away magical energy.",

"Another dark, cave-dwelling race, likewise unhampered by darkness attacks, the Dark Elves have a long tradition and knowledge of magic.  They have an inherent magic missile attack available to them at a low level.  With their keen sight, they also learn to see invisible things as their relatives High-Elves do, but at a higher level.",

"A humanoid race with dragon-like attributes.  As they advance levels, they gain new elemental resistances (up to Poison Resistance), and they also have a breath weapon, which becomes more powerful with experience.  The exact type of the breath weapon depends on the Draconian's class and level.  With their wings, they can easily escape any pit trap unharmed.",

"A secretive and mysterious ancient race.  Their civilization may well be older than any other on our planet, and their intelligence and wisdom are naturally sustained, and are so great that they enable Mind Flayers to become more powerful spellcasters than any other race, even if their physical attributes are a good deal less admirable.  As they advance levels, they gain the powers of See Invisible and Telepathy.",

"A demon-creature from the nether-world, naturally resistant to fire attacks, and capable of learning fire bolt and fire ball attacks.  They are little loved by other races, but can perform fairly well in most professions.  As they advance levels, they gain the powers of See Invisible.",

"A Golem is an artificial creature, built from a lifeless raw material like clay, and awakened to life.  They are nearly mindless, making them useless for professions which rely on magic, but as warriors they are very tough.  They are resistant to poison, they can see invisible things, and move freely.  At higher levels, they also become resistant to attacks which threaten to drain away their life force.  Golems gain very little nutrition from ordinary food, but can absorb mana from staves and wands as their power source.  Golems also gain a natural armor class bonus from their tough body.",

"There are two types of skeletons: the ordinary, warrior-like skeletons, and the spell-using skeletons, which are also called liches.  As undead beings, skeletons need to worry very little about poison or attacks that can drain life.  They do not really use eyes for perceiving things, and are thus not fooled by invisibility.  Their bones are resistant to sharp shrapnel, and they will quickly become resistant to cold.  Although the magical effects of these will affect the skeleton even without entering the skeleton's (non-existent) belly, the potion or food itself will fall through the skeleton's jaws, giving no nutritional benefit.  They can absorb mana from staves and wands as their energy source.",

"Much like Skeletons, Zombies too are undead horrors: they are resistant to life-draining attacks, and can learn to restore their life-force.  Like skeletons, they become resistant to cold-based attacks (actually earlier than skeletons), resist poison and can see invisible.  While still vulnerable to cuts (unlike skeletons), Zombies are resistant to Nether.  Like Golems, they gain very little nutrition from the food of mortals, but can absorb mana from staves and wands as their energy source.",

"One of the mightier undead creatures, the Vampire is an awe-inspiring sight.  Yet this dread creature has a serious weakness: the bright rays of sun are its bane, and it will need to flee the surface to the deep recesses of earth until the sun finally sets.  Darkness, on the other hand, only makes the Vampire stronger.  As undead, the Vampire has a firm hold on its life force, and resists nether attacks.  The Vampire also resists cold and poison based attacks.  It is, however, susceptible to its perpetual hunger for fresh blood, which can only be satiated by sucking the blood from a nearby monster.",

"Another powerful undead creature: the Spectre is a ghastly apparition, surrounded by an unearthly green glow.  They exist only partially on our plane of existence: half-corporeal, they can pass through walls, although the density of the wall will hurt them in the process of doing this.  As undead, they have a firm hold on their life force, see invisible, and resist poison and cold.  They also resist nether.  At higher levels they develop telepathic abilities.  Spectres make superb spellcasters, but their physical form is very weak.  They gain very little nutrition from the food of mortals, but can absorb mana from staves and wands as their energy source.",

"One of the several fairy races, Sprites are very small.  They have tiny wings and can fly over traps that may open up beneath them.  They enjoy sunlight intensely, and need worry little about light based attacks.  Although physically among the weakest races, Sprites are very talented in magic, and can become highly skilled wizards.  Sprites have the special power of spraying Sleeping Dust, and at higher levels they learn to fly faster.",

 "This race is a blasphemous abomination produced by Chaos.  It is not an independent race but rather a humanoid creature, most often a human, twisted by the Chaos, or a nightmarish crossbreed of a human and a beast.  All Beastmen are accustomed to Chaos so much that they are untroubled by confusion and sound, although raw logrus can still have effects on them.  Beastmen revel in chaos, as it twists them more and more.  Beastmen are subject to mutations: when they have been created, they receive a random mutation.  After that, every time they advance a level they have a small chance of gaining yet another mutation.",

"The Ents are a powerful race dating from the beginning of the world, oldest of all animals or plants who inhabit Arda.  Spirits of the land, they were summoned to guard the forests of Middle-earth.  Being much like trees they are very clumsy but strong, and very susceptible to fire.  They gain very little nutrition from the food of mortals, but they can absorb water from potions as their nutrition.",

"Archons are a higher class of angels.  They are good at all skills, and are strong, wise, and are a favorite with any people.  They are able to see the unseen, and their wings allow them to safely fly over traps and other dangerous places.  However, belonging to a higher plane as they do, the experiences of this world do not leave a strong impression on them and they gain levels slowly.",

"Balrogs are a higher class of demons.  They are strong, intelligent and tough.  They do not believe in gods, and are not suitable for priest at all.  Balrog are resistant to fire and nether, and have a firm hold on their life force.  They also eventually learn to see invisible things.  They are good at almost all skills except stealth.  They gain very little nutrition from the food of mortals, and need human corpses as sacrifices to regain their vitality.",

"Dunedain are a race of hardy men from the West.  This elder race surpasses human abilities in every field, especially constitution.  However, being men of the world, very little is new to them, and levels are very hard for them to gain.  Their constitution cannot be reduced. ",

"Shadow Fairies are one of the several fairy races.  They have wings, and can fly over traps that may open up beneath them.  Shadow Fairies must beware of sunlight, as they are vulnerable to bright light.  They are physically weak, but have advantages in using magic and are amazingly stealthy.  Shadow Fairies have a wonderful advantage in that they never aggravate monsters (If their equipment normally aggravates monsters, they only suffer a penalty to stealth, but if they aggravate by their personality itself, the advantage will be lost).",

"A Kutar is an expressionless animal-like living creature.  The word 'kuta' means 'absentmindedly' or 'vacantly'.  Their absentmindedness hurts their searching and perception skills, but renders them incapable of being confused.  Their unearthly calmness and serenity make them among the most stealthy of any race.  Kutars, although expressionless, are beautiful and so have a high charisma.  Members of this race can learn to expand their body horizontally.  This increases armour class, but renders them vulnerable to magical attacks.",

"An android is a artificial creation with a body of machinery.  They are poor at spell casting, but they make excellent warriors.  They don't acquire experience like other races, but rather gain in power as they attach new equipment to their frame.  Rings, amulets, and lights do not influence growth.  Androids are resistant to poison, can move freely, and are immune to life-draining attacks.  Moreover, because of their hard metallic bodies, they get a bonus to AC.  Androids have electronic circuits throughout their body and must beware of electric shocks.  They gain very little nutrition from the food of mortals, but they can use flasks of oil as their energy source."

#endif
};

static cptr class_jouhou[MAX_CLASS] =
{
#ifdef JP
"あなたは武器の扱いに熟達し、圧倒的な剛腕で道を切り開くことができます。直接殴る斬るだけが能ではなく弓矢や投擲の技術も習得しています。1対1の戦いで敵に後れを取る事はほとんどありませんが、戦闘以外のことは極めて不得手です。中盤以降のダンジョンで生き延びるためには、苦手な魔道具や隠密能力などをどうにかして補う事が必要です。",
  
"あなたは知識を力とする道を選び、魔法の奥義を究めるために研究と実践を日々積み重ねています。序盤は貧弱な身体能力のために苦戦しますが、終盤は強力かつ多様な戦い方が可能になることでしょう。機知に富んでおり、探索や魔道具の使用が得意です。レベルが上がると魔道具の魔力を吸収することができるようになります。全12領域の魔法の中から2領域の魔法を習得できます。第一領域の分類により、魔術師、妖術師、道師と呼び方が変わります。第二領域はいつでも変更可能ですが、第二領域が第一領域と別の分類の場合難易度が大幅に上昇してしまいます。",
  
"あなたは神に仕えて儀式や祈祷を行うことを生業としています。神の助けを得て仙術に属する魔法を2領域行使可能です。信仰の賜物により魔法防御力に優れ、レベルが上がると恐怖などに対する耐性を得ます。日々の修行の一環としてある程度の武芸もこなせますが、それほどの達人にはなれません。また、鈍器と棒以外の祝福されていない武器を装備すると神の不興を買い魔法の行使などに差し障りが出ます。成長はかなり遅めですが、神にアイテムを捧げて祈ることにより幾許かの経験値を得ることができます。なお、もしかしたらあなたは邪神や祟り神と呼ばれる神に仕えているのかもしれません。その場合は仙術でなく妖術に属する魔法を習得し、祝福された武器を装備するとペナルティを受けます。",
  
"あなたは宝物を求めて怪物の棲家や遺跡を探検する、いわゆるトレジャーハンターです。隠密や解錠や投擲など様々な技能に優れ、探索に向いた便利な特技を持ちます。職業柄しばしば怪物や遺跡の番人と戦闘になるため白兵戦もそこそここなせますが、純戦士ほどの腕っ節の強さはありません。魔法を一領域習得可能ですがその学習は遅く、あまり強力な魔法は使えません。短剣の扱いが得意で短剣を装備していると攻撃回数が上昇します。短剣のほかにはクロスボウの扱いも得意です。それ以外の大きな武器の扱いは苦手です。",
  
"あなたは森の奥深くで獣のように狩りをして暮らしています。過酷な環境で育ったあなたは白兵戦と射撃の両方に秀でています。また先達から伝えられた自然の力を借りる技術を修めており自然か変容の魔法を行使可能です。成長すると麻痺や毒の耐性を得て、さらにモンスターの詳細な情報を調査する特技を使えるようになります。隠密行動は得意ですが小手先の繊細な技術についてはやや不得手です。",
  
"あなたは聖域を守るために戦う高潔な戦士です。高い白兵能力を持ち、特に剣と槍の扱いを得意とします。魔法は使えませんが外敵から身を守るためのいくつかの技を習得できます。祝福された武器を装備すると攻撃力が増加しますが、呪われた武器を装備すると攻撃力が大幅に低下します。騎乗技能の適性もありますが混沌の勢力の魔獣を操ることは大の苦手です。レベルが上昇すると恐怖と破邪攻撃に対する耐性を得ます。",
  
"あなたは様々な事柄に対する深い知識を修め、それを後進に伝えることを使命としています。魔法を2領域習得可能で、のみならず武器や戦闘技術についても幅広く習得した万能職です。しかし肉体能力にも魔法の扱いにも突出したところがなく、技能の成長にもあまり伸びしろがない器用貧乏でもあります。中盤以降のダンジョンで生き残るためには手に入るものを全て使いこなす柔軟さが求められます。",
  
"あなたは主に仕えて日々の雑務を行い、侵入者があれば迎撃し一命下れば敵地を強襲する頼れる従者です。様々な武器を小器用に扱い探索などの技能もやや高く、いくつかの便利な小技を習得します。さらに魔法職ほどではないものの魔法も1領域行使可能な万能型職業です。ただしメイド（執事）たるもの常にメイド服（スーツ）を着用しなければなりません。優秀な防具の確保には苦労することでしょう。レベルアップのとき主から褒美を受け取ります。しかし主の機嫌が悪いと理不尽な罰を受けることがあります。",
  
"あなたは武器に頼らず身一つで戦い抜くことを好み、日々肉体と技の鍛錬に励んでいます。その鍛錬の成果は見事なもので、素手であるにも関わらず重装備の戦士に匹敵する攻撃力を叩き出します。また厳しい修行により「気」を扱う方法を習得しており、様々な常人離れした技を使うことができます。ただし盾や重い鎧を装備すると格闘に差し障りが出るため、装備を整えるのには苦労することでしょう。罠解除や魔道具といった技術はあまり得意ではありません。",
  
"あなたはいわゆる超能力者で、精神の力で様々な奇跡的なことを成し遂げます。その能力は幅広く、とくに防御と感知に関しては生半可な魔法を圧倒します。超能力の習得や行使には知能を要さず、レベルと賢さのみが要求されます(超能力は魔法と同じコマンドから使用します)。体はやや華奢ですが肉弾戦もそこそここなせ、扱いやすい万能型と言えるでしょう。レベルが上がると混乱耐性やテレパシーの能力を獲得します。成長は早めですがスコアは低めです。",
  
"あなたは知識を力とする道を選び、そしてただ一領域の魔法を極めることを決心しました。習得できる魔法は一領域だけであるものの、他のどの職よりも巧みにその領域の魔法を扱います。通常の魔法職よりもかなりMPが多く、一定レベルに達すると選択した領域に応じた独特の能力を得ることができます。ハイメイジの呼称もまたメイジのように領域ごとに変化します。",
  
"あなたは幻想郷に暮らす普通の住人です。人里の豆腐屋かもしれませんし、花畑で遊ぶ妖精かもしれません。何らかの事情でダンジョン探索に身を投じましたが戦闘に関してはほとんど何の取り柄もありません。1領域のみ魔法の習得が可能ですが強力な魔法はあなたの手には余ります。あなたの冒険は苦難に満ちたものになるでしょう。なお、ゲーム開始時に職業名を好きなように再設定可能です。",
  
"あなたは山に籠もり、肉体と精神の鍛錬のために修行に明け暮れています。刀と棒の扱いに秀で、素手での格闘も得意です。棒を装備しているときには格闘の追加攻撃が出やすくなります。日々の修行の賜物で仙術に属する魔法を一領域行使可能ですが、ひょっとすると外法に手を出して変容や死霊の領域を習得しているかもしれません。レベルが上がると恐怖や混乱への耐性を身につけ、さらに精神を集中して素早くMPを回復することができるようになります。",
  
"あなたは並々ならぬ努力によって剣と魔法の両方を習得した戦士です。白兵能力は純戦士に遠く及ばず魔法の習熟も純魔法職に数歩譲りますが、剣に魔法の効果を乗せて特殊な攻撃をする特技を持ちます。魔術かあるいは暗黒・混沌の妖術から一領域の魔法を習得でき、魔法剣の効果は習得した魔法領域で変わります。剣の扱いには極めて秀でますがほとんど剣しか扱えません。レベルが上がると透明視認と急回復の能力を獲得します。なお、成長はやや遅めです。",
  
"あなたは幻想郷の弾幕決闘に魅せられ、あらゆる弾幕を自ら再現しようと日夜研究を重ねている変わり者のメイジです。モンスターの特技を観察し、一定の習得度に達した特技を使用することができます。習得のしやすさは特技のレベルと難易度、自分のレベルと知能、モンスターとの距離で変わります。なお「特技の観察」とは大抵の場合その攻撃を自ら受けることを意味します。最終的には凄まじい強さになれる可能性がありますが、そこまでの道のりは苦労と危険の連続になるかもしれません。",
  
"あなたは弓やクロスボウの扱いに特化した戦士です。その射撃の手練は凄まじいもので、常人の数倍の速度で正確無比な射撃を行うことができます。しかし接近戦での強さは他の戦士系職業に大きく劣ります。あなたは手先が器用でその辺りに生えている木から矢を作ることができ、アーチャーギルドに矢と素材を持ち込んで加工することでさらに強力な矢を作ることができます。隠密や探索などその他の技能は概ね平均的なレベルです。レベルアップにより透明視認や盲目耐性の能力を得ます。",
  
"あなたは特殊な術師です。魔法書を全く読むことができませんが、その代わりにマジックアイテムの扱いにかけては誰よりも優れています。杖、魔法棒、ロッドといった魔道具を体に取り込んで使用する能力を持っており、取り込んだ魔道具は時間経過で使用回数が回復します。また充填や発動に関するいくつかの特技を習得します。肉弾戦はそれほど得意ではないため、強力な魔道具が手に入るまでは成長が伸び悩むかもしれません。",
  
"あなたは機械技師で、複雑怪奇な機械を操って戦局を切り開きます。機械の作成や弾薬燃料の調達はエンジニアギルドで行います。あなたは飛行可能なバックパックを持っておりその中に機械を格納して使用します。機械の種類は多いため格納する機械は吟味する必要があります。機械は極めて強力ですが、何をするにもとにかくお金がかかるという大きな欠点があります。機械の作成には知能と器用さが要求されますが、何より必要なのは戦利品を高く売って金策をするための魅力かもしれません。多くの機械は水と電撃が大敵で、これらの攻撃を受けると一時的に使用不能になることがあります。あなた自身の戦闘能力は平凡です。罠解除とクロスボウが得意ですが魔道具の使用は大の苦手です。",
  
"あなたは図書館に勤務しています。図書館の主に仕えて雑務をこなし本泥棒を迎撃するのが仕事です。書物について広い知識を持っており、驚くべきことに全ての領域の二冊目までの魔法を最初から記憶しています。レベルが足りていて本を持っていれば魔法を使用可能です。しかし魔法の習熟度は本格的な魔法職には遠く及ばず、三冊目以降の強力な魔法書は一切使えません。武術についてもそれなりに心得はありますがこちらも本職には一、二歩譲り、あまり大きな武器を振り回すのは苦手です。冒険の序盤は何不自由なくこなせますが、終盤で強敵を相手にするのは苦労するかもしれません。レベルがかなり上がるとMPを多めに消費して呪文を高速詠唱する特技を身につけます。",
  
"あなたは剣の道を極めるために日々技を磨いています。刀の扱いにかけては右に出るものはいません。魔法は使えませんが、先人たちの残した秘伝書を読むことで「必殺剣」を習得することが可能です。また「型」という特殊な構えを取ることにより一風変わった戦い方をすることができます。二刀流にも高い適性を持ちますが、探索や魔道具などは苦手です。",
  
"あなたは銃火器で武装した戦士です。	銃とナイフの扱いに優れ、格闘・開錠・隠密にも適性があります。一方で魔道具の扱いと魔法防御はかなり苦手です。一定レベルになるごとに技能習得ポイントを得て、ポイントを消費して幾つかのカテゴリから技能を学ぶことができます。ポイントの割り振りによりダンジョン攻略の戦略がある程度変わります。あなたは幻想郷の住人から見ると明らかに異質な装いをしているため、街では警戒されてしまうかもしれません。",
  
"あなたは薬を作って売ることで生計を立てています。ダンジョンなどで素材と調剤レシピを入手し、街の調剤室を借りて薬を調合します。戦闘はあまり得意でないため、強敵と戦うときは爆薬や強化薬などを事前に調合しておく必要があるでしょう。あなたはザックとは別に素材や薬を入れる薬箱を常に持ち歩いています。薬箱の中のアイテムは厳重に保護されており敵の攻撃で破壊されませんが、一度薬箱から出さないと使用できません。レベルが上がるとザックの中の薬が冷気攻撃などで破壊されにくくなります。薬の合成には知能が必要で、材料の採取のためには隠密能力や探索能力、ある程度の戦闘の準備が必要です。",
  
"あなたは乗馬に熟達した騎士です。その乗馬の技術は見事なもので、馬に留まらず怪鳥や巨獣、果ては竜まで乗りこなします。強力なモンスターに乗れれば極めて心強い支援が得られることでしょう。槍や長柄武器の扱いが得意で弓の扱いも巧みです。また乗馬に向かない武器でもそれなりに扱えます。しかし戦士と同様に魔法は一切使えず魔道具の扱いも苦手です。",
  
"あなたは道具と付喪神の専門家で、武器を付喪神に変化させる特殊な技を持っています。付喪神となった武器は宙に浮き、あなたのしもべとなって戦います。通常の配下と違い、付喪神の攻撃であなた自身が経験値とアイテムを得ることができます。あなたの装備している武器は妖器化して最大二回新たな能力を獲得することがあります。あなたは鑑定や罠解除や魔道具使用に秀でており、呪われた装備品を解呪せずに外せます。しかしあなた自身の戦闘能力はそれほど高くありません。ダンジョンの深層では地形や相手を選ぶ慎重な立ち回りが要求されます。",

"あなたは拾ったり安く買い取った雑多な物を売って暮らしています。その目利きは確かで、鑑定に関してはどの職業よりも優れています。魔道具や探索の技能に優れ、重い荷物を運ぶことができ、店での交渉が得意です。またあなたには魔法の装備品を作り変える技術があり、ギルドで素材となるアイテムから能力を抽出し別の武具に付加することができます。ただしあなたの基本的な戦闘能力はそれほど高いものではなく、魔法も一切使用できません。ダンジョンの奥に辿り着くには根気強く素材を集めて装備を鍛え、戦力の底上げを図る必要があります。あなたは冒険用のザックに加えて拾ったものを運ぶための道具箱を持っています。",

"あなたは魔法の装飾品を扱う宝石細工師です。宝石や装飾品から魔力を抽出し、他の装飾品に特別な宝飾を施すことで魔力を付与する技術を持っています。作業は街の宝飾ギルドで行います。あなたは手先が器用で魔法の道具の扱いに優れますが、近接戦闘能力は一般人と変わりません。またあなたは魔法使いでもあり魔法を一領域習得可能ですが純粋なメイジに比べるとやや素質に劣ります。修得する魔法領域は慎重に選択する必要があるでしょう。レベルが上がると手持ちの宝石から魔力を吸収してMPを回復する特技を習得します。",

"あなたは特殊な訓練を積んだ恐るべき暗殺者です。暗闇の中でも周囲を見通すことができ、また暗闇の中では敵に見つからずに行動できます。短剣や素手の一撃で敵を無力化し、高い投擲技能を持ち、さらには忍術と呼ばれる特殊技能を使いこなします。攻撃においてはその強さを遺憾なく発揮しますが、一方で防御面にはやや不安があります。耐性とACには優れているものの体力に欠け、盾や重い武器防具を一切装備できません。忍術による防御も当たり外れのあるリスキーなものです。忍術の使用には主に器用さあるいは知能が影響します。",

"あなたは耳の早い商売人あるいは投機家です。アビリティカードを見て将来の流行を確信し、カードを買い占めて一儲けしようと企んでいます。しかし同じことを考える者が次々に現れ、価格の高騰は必至です。とにかく急いでお金を貯め、守矢神社の近くのカード販売所、偽天棚の賭場、あるいはカードを持つ人妖からカードを買い漁りましょう。あなたには本職に遠く及ばないまでも多少の近接戦や魔法の心得があります。またカードケースを持っておりこの中にカードを8種類まで格納することができます。あなたに必要な能力はカードを発動するための魔道具技能、アビリティカードの威力を高めたり効率よく金策を行うための魅力です。",

"あなたは気が付くと何処とも知れぬ草原に立っていました。人の手の入っていない自然の風景に心洗われるのも束の間、この場所はこれまでの人生で培ってきた常識が全く通じないこと、怪物が闊歩しており危険に満ちていること、そして自分が全く無力であることを知ります。あなたは幸運にも人の住む場所に辿り着きました。幸い言葉は通じるようです。里ではあなたのような人が来ることは初めてではないらしく、親切な人が博麗神社に行けば外へ送り返してくれると教えてくれました。しかし神社への道程は妖怪の巣窟だそうです。何かの異変が起こっているらしく里はざわついていますが、あなたにとっては何が異変で何が日常風景なのかすら定かではありません・・",
"","",//ユニーククラスの説明文はunique_player_table[]
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","",//class155
"あなたは危険な魔法の森に居を構える変わり者です。近所の魔法使いが森の蜂を育てて蜂蜜を集めているのを見て真似してみることにしました。森の魔力で変質した蜂たちはあなたといくらかの意思疎通ができ、蜂蜜を集める以外にも色々と助けになってくれます。あなたには武器と魔法の心得が多少ありますがどちらも本職には遠く及びません。蜂たちをうまく使って生き延びましょう。ただし蜂への命令に失敗すると蜂は怒って攻撃してくるかもしれません。あなたに必要な能力は知能と魅力です。",//156:養蜂家
"","","","","","","",

#else

"A Warrior is a hack-and-slash character, who solves most of his problems by cutting them to pieces, but will occasionally fall back on the help of a magical device.  Unfortunately, many high-level devices may be forever beyond their use.",

"A Mage is a spell caster that must live by his wits as he cannot hope to simply hack his way through the dungeon like a warrior.  In addition to his spellbooks, a mage should carry a range of magical devices to help him in his endeavors which he can master far more easily than anyone else.  A mage's prime statistic is Intelligence as this determines his spell casting ability. ",

"A Priest is a character devoted to serving a higher power.  They explore the dungeon in the service of their God.  Since Priests receive new prayers as gifts from their patron deity, they cannot choose which ones they will learn.  Priests are familiar with magical devices which they believe act as foci for divine intervention in the natural order of things.  A priest wielding an edged weapon will be so uncomfortable with it that his fighting ability.  A Priest's primary stat is Wisdom since this determine his success at praying to his deity. ",

"A Rogue is a character that prefers to live by his cunning, but is capable of fighting his way out of a tight spot.  Rogues are good at locating hidden traps and doors and are the masters of disarming traps and picking locks.  A rogue has a high stealth allowing him to sneak around many creatures without having to fight, or to get in a telling first blow.  A rogue may also backstab a fleeing monster.  Intelligence determines a Rogue's spell casting ability.",

"A Ranger is a combination of a warrior and a mage who has developed a special affinity for the natural world around him.  He is a good fighter and also good about a missile weapon such as a bow.  A ranger has a good stealth, good perception, good searching, a good saving throw and is good with magical devices.  Intelligence determines a Ranger's spell casting ability.",

"A Paladin is a combination of a warrior and a priest.  Paladins are very good fighters, but not very good at missile weapons.  A paladin lacks much in the way of abilities.  He is poor at stealth, perception, searching, and magical devices but has a decent saving throw due to his divine alliance.  Wisdom determines a Paladin's success at praying to his deity.",

"A Warrior-Mage is precisely what the name suggests: a cross between the warrior and mage classes.  While their brothers, the rangers, specialize in Nature magic and survival skills, true Warrior-Mages attempt to reach the best of both worlds.  As warriors they are much superior to the usual Mage class.  Intelligence determines a Warrior-Mage's spell casting ability.",

"Chaos Warriors are the feared servants of the terrible Demon Lords of Chaos.  Every Chaos Warrior has a Patron Demon and, when gaining a level, may receive a reward from his Patron.  He might be healed or polymorphed, his stats could be increased, or he might be rewarded with an awesome weapon.  On the other hand, the Patrons might surround him with monsters, drain his stats or wreck his equipment or they might simply ignore him.  The Demon Lords of Chaos are chaotic and unpredictable indeed.  The exact type of reward depends on both the Patron Demon (different Demons give different rewards) and chance.",

"The Monk character class is very different from all other classes.  Their training in martial arts makes them much more powerful with no armor or weapons.  To gain the resistances necessary for survival a monk may need to wear some kind of armor, but if the armor he wears is too heavy, it will severely disturb his martial arts maneuvers.  As the monk advances levels, new, powerful forms of attack become available.  Their defensive capabilities increase likewise, but if armour is being worn, this effect decreases.  Wisdom determines a Monk's spell casting ability.",

"The Mindcrafter is a unique class that uses the powers of the mind instead of magic.  These powers are unique to Mindcrafters, and vary from simple extrasensory powers to mental domination of others.  Since these powers are developed by the practice of certain disciplines, a Mindcrafter requires no spellbooks to use them.  The available powers are simply determined by the character's level.  Wisdom determines a Mindcrafter's ability to use mind powers.",

"High-mages are mages who specialize in one particular field of magic and learn it very well - much better than the ordinary mage.  For the price of giving up a second realm of magic, they gain substantial benefits in the mana costs, minimum levels, and failure rates in the spells of the realm of their specialty.  A high mage's prime statistic is intelligence as this determines his spell casting ability. ",

"Tourists have visited this world for the purpose of sightseeing.  Their fighting skills is bad, and they cannot cast powerful spells.  They are the most difficult class to win the game with.  Intelligence determines a tourist's spell casting ability.",

"Imitators have enough fighting skills to survive, but rely on their ability to imitate monster spells.  When monsters in line of sight use spells, they are added to a temporary spell list which the imitator can choose among.  Spells should be imitated quickly, because timing and situation are everything.  An imitator can only repeat a spell once each time he observes it.  Dexterity determines general imitation ability, but a stat related to the specific action is often also taken into account.",

"Beastmasters are in tune with the minds of the creatures of the world of Hengband.  They are very good at riding, and have enough fighting ability.  They use monsters which summoned or dominated by him as his hands and feet.  Beastmasters can cast trump magic, and very good at summoning spell, but they can not summon non-living creatures.  Charisma determines a Beastmaster's spell casting ability.",

"Sorcerers are the all-around best magicians, being able to cast any spell from most magic realms without having to learn it.  On the downside, they are the worst fighters in the dungeon, being unable to use any weapon but a Wizardstaff.",

"Archers are to bows what warriors are to melee.  They are the best class around with any bow, crossbow, or sling.  They need a lot of ammunition, but will learn how to make it from junk found in the dungeon.  An archer is better than a warrior at stealth, perception, searching and magical devices.",

"Magic-Eaters can absorb the energy of wands, staffs, and rods, and can then use these magics as if they were carrying all of these absorbed devices.  They are middling-poor at fighting.  A Magic-Eater's prime statistic is intelligence.",

"Bards are something like traditional musicians.  Their magical attacks are sound-based, and last as long as the Bard has mana.  Although a bard cannot sing two or more songs at the same time, he or she does have the advantage that many songs affect all areas in sight.  A bard's prime statistic is charisma.",

"Red-Mages can use almost all spells from lower rank spellbooks of most realms without having to learn it.  At higher level, they develop the powerful ability \"Double Magic\".  However, they have large penalties in the mana costs, minimum levels, and failure rates of spells, and they cannot use any spells from higher rank spellbooks.  They are not bad at using magical devices and magic resistance, and are decent fighter, but are bad at other skills.  A red-mage's prime statistic is intelligence.",

"Samurai, masters of the art of the blade, are the next strongest fighters after Warriors.  Their spellpoints do not depend on level, but depend solely on wisdom, and they can use the technique Concentration to temporarily increase SP beyond its usual maximum value.  Samurai are not good at most other skills, and many magical devices may be too difficult for them to use.  Wisdom determines a Samurai's ability to use the special combat techniques available to him.",

"A ForceTrainer is a master of the spiritual Force.  They prefer fighting with neither weapon nor armor.  They are not as good fighters as are Monks, but they can use both magic and the spiritual Force.  Wielding weapons or wearing heavy armor disturbs use of the Force.  Wisdom is a ForceTrainer's primary stat.",

"A Blue-Mage is a spell caster that must live by his wits, as he cannot hope to simply hack his way through the dungeon like a warrior.  A major difference between the Mage and the Blue-Mage is the method of learning spells: Blue-Mages may learn spells from monsters by activating his Learning ability.  A Blue-Mage's prime statistic is Intelligence as this determines his spell casting ability. ",

"Cavalry ride on horses into battle.  Although they cannot cast spells, they are proud of their overwhelming offensive strength on horseback.  They are good at shooting.  At high levels, they learn to forcibly saddle and tame wild monsters.  Since they take pride in the body and the soul, they don't use magical devices well.",

"A Berserker is a fearful fighter indeed, immune to fear and paralysis.  At high levels, Berserkers can reflect bolt spells with their tough flesh.  Furthermore, they can fight without weapons, can remove cursed equipment by force, and can even use their special combat techniques when surrounded by an anti-magic barrier.  Berserkers, however, cannot use any magical devices or read any scrolls, and are hopeless at all non-combat skills.  Since Berserker Spectres are quite easy to *win* with, their scores are lowered.",

"A Weaponsmith can improve weapons and armors for him or herself.  They can extract the essences of special effects from weapons or armors which have various special abilities, and can add these essences to another weapon or armor.  They are good at fighting, but cannot cast spells, and are poor at skills such as stealth or magic defense.",

"Mirror-Masters are spell casters; like other mages, they must live by their wits.  They can create magical mirrors, and employ them in the casting of Mirror-Magic spells.  A Mirror-Master standing on a mirror has greater ability and, for example, can perform quick teleports.  The maximum number of Magical Mirrors which can be controlled simultaneously depends on the level.  Intelligence determines a Mirror-Master's spell casting ability.",

"A Ninja is a fearful assassin lurking in darkness.  He or she can navigate effectively with no light source, catch enemies unawares, and kill with a single blow.  Ninjas can use Ninjutsu, and are good at locating hidden traps and doors, disarming traps and picking locks.  Since heavy armors, heavy weapons, or shields will restrict their motion greatly, they prefer light clothes, and become faster and more stealthy as they gain levels.  A Ninja knows no fear and, at high level, becomes almost immune to poison and able to see invisible things.  Dexterity determines a Ninja's ability to use Ninjutsu.",

"Snipers are good at shooting, and they can kill targets by a few shots. After they concentrate deeply, they can demonstrate their shooting talents. You can see incredibly firepower of their shots."
#endif
};


static cptr seikaku_jouhou[MAX_SEIKAKU] =
{
#ifdef JP
"あなたは特に特筆するべき部分がない性格です。あらゆる技能を平均的にこなします。",

"あなたは考えるより先に手が出るタイプです。肉体的な能力や技能が上昇します。しかし、魔法に関係する能力や技能は劣り、戦士よりのステータスを持ちます。",

"あなたは頭の回転が速くそして思慮深い性格です。肉体的な能力は下がりますが、知能や魔法に関係する技能は上昇し、メイジよりのステータスを持ちます。",

"あなたは少し鈍いところがありますが少々のことでは動じません。耐久力にやや優れますが不器用です。",

"あなたはやや落ち着きがありませんが目端が利くタイプです。どのスキルも比較的うまくこなしますが、肉体的な能力はやや低くなります。",

"あなたは一度これと決めたら後先考えません。戦闘力、魔法能力の両方が上昇しますが、魔法防御、ＨＰといった能力は悪くなります。",

"あなたはシューティングの申し子です。魔法だろうと射撃だろうととにかく何かをぶっ放すことにかけては天下一品です。しかし自分の身を守ることなど全く眼中にありません。撃たれる覚悟はできています。",

"あなたはすっきりと垢抜けた出で立ちをしており何をするにもスマートです。しかし力仕事や隠密行動はあまり好みではありません。",

"幻想郷でキャノンボールが開催されました。鉄獄100階を最も早く駆け抜けたものが優勝です。あなたは高い能力と技能を持ちますが、ダンジョンの全ての存在があなたの邪魔をしようと立ち塞がります。この性格を選んで短時間で勝利するとスコアが上がります。",

"あなたは能力は劣っていますがなぜか何をやっても何となくうまくいってしまうタイプです。",

"あなたはじっくりと物事にとりくむ慎重な性格です。高い耐久力を持ちますが自分から行動するのは苦手で、多くの技能は低くなってしまいます。",

"いかさまは、初心者の練習用の性格です。あらゆる能力が高くなっています。この性格を使えば勝利者になることは容易ですが、勝利しても全く自慢になりません。",

"(クラスごとに個別設定する特殊性格　このメッセージは表示されない)",

"やめときな！気がふれるぜ？",

#else

"\"Ordinary\" is a personality with no special skills or talents, with unmodified stats and skills.",

"\"Mighty\" raises your physical stats and skills, but reduces stats and skills which influence magic.  It makes your stats suitable for a warrior.  Also it directly influences your hit-points and spell fail rate.",

"\"Shrewd\" reduces your physical stats, and raises your intelligence and magical skills.  It makes your stats suitable for a mage.  Also it directly influences your hit-points and spell fail rate.",

"\"Pious\" deepens your faith in your God.  It makes your physical ability average, and your stats suitable for priest. ",

"\"Nimble\" renders you highly skilled comparatively well, but reduces your physical ability. ",

"\"Fearless\" raises both your melee and magical ability.  Stats such as magic defense and constitution are reduced.  Also it has a direct bad influence on your hit-points.",

"\"Combat\" gives you comparatively high melee and shooting abilities, and average constitution.  Other skills such as stealth, magic defence, and magical devices are weakened.  All \"Combat\" people have great respect for the legendary \"Combat Echizen\".\n\
(See \"Death Crimson\" / Ecole Software Corp.)",

"A \"Lazy\" person has no good stats and can do no action well.  Also it has a direct bad influence on your spell fail rate.",

"\"Sexy\" rises all of your abilities, but your haughty attitude will aggravate all monsters.  Only females can choose this personality.",

"A \"Lucky\" man has poor stats, equivalent to a \"Lazy\" person.  Mysteriously, however, he can do all things well.  Only males can choose this personality.",

"A \"Patient\" person does things carefully.  Patient people have high constitution, and high resilience, but poor abilities in most other skills.  Also it directly influences your hit-points.",

"\"munchkin\" is a personality for beginners.  It raises all your stats and skills.  With this personality, you can win the game easily, but gain little honor in doing so."
#endif
};

static cptr seikaku_special_jouhou[MAX_SEIKAKU_SPECIAL] =
{
	"この性格は特定のクラスの専用特殊性格です。このクラスではこの性格は選択できません。",
	//チルノ「日焼けした」
	"あなたは氷の妖精だというのになぜかこんがり日焼けしました。火炎弱点を克服し、全能力が大幅に上昇し、さらに特技が強化され、あなたは全能感に満ちあふれています。ただしあなたの冒険のスコアの半分はどこかに消えてしまいます。あなたの背中にはいつの間にか謎の扉が開いていますが、あなたがそれに気付くことはありません。",
	//パチュリー「書痴」
	"あなたは手当たり次第の本を一心不乱に読み漁り、ついに魔術だけでなく仙術と妖術の魔法までも全て習得しました。しかし無理が祟ったあなたはいつにも増して健康を損ない、今の身体能力は雑魚妖精にすら劣ります。",
	//成美「理由なく好戦的な」
	"あなたは森で一人静かに暮らしていましたが、あるとき突然膨大な魔力が湧き上がるのを感じました。誰でもいいからこの力を試したくて仕方がありません。今のあなたは近接攻撃能力が強化され、MP最大値が増え、特技も強力な攻撃寄りのものに変わっています。ただしあなたの冒険のスコアの25%はどこかに消えてしまいます。あなたの背後では謎の少女が笹をガサガサ振りながら踊っていますが、あなたがそれに気付くことはありません。",
	//うどんげ「夢の世界の」
	"あなたは夢の世界の鈴仙・優曇華院・イナバです。	完全憑依異変の影響で夢の世界から追い出され、この機会にあなたの本体にストレスをかける者を一人残らず叩きのめそうと目論んでいます。この性格を選択すると通常の性格から特技が変化し、薬や銃の扱いに長けたものになります。",
	//菫子「悪夢に悩まされる」
	"あなたはこのところ連夜の悪夢に悩まされています。普段はそれなりに友好的なはずの幻想郷の住人たちが一人残らず悪意をもって襲い掛かってくるのです。	しかしある夜、あなたの元に絶対秘神を名乗る人物が現れて力を貸してくれました。新たな特技を使って終わらない悪夢を打開しましょう。秘神があなたに授ける特技はその時その時でランダムに決まり、あなたが好きなように名前をつけることができます。",
	//針妙丸「意外に顔が広い」
	"あなたは小さな体に眩いばかりの理想と情熱を秘めており、その輝きは多くの人妖の心を動かします。この性格でゲームを開始すると、一部クエストを除き幻想郷の住人全てが友好的になります。またこの性格専用の特技で友好的な味方を際限なく呼び出すことができます。友軍の攻撃に巻き込まれないよううまく立ち回って敵に止めを刺しましょう。",
	//赤蛮奇「スマホ首」
	"あなたはこのところ歩く時間も惜しんで読書に没頭しているのですが、なぜか周囲の目が冷たい気がします。	この性格では通常の性格に比べて近接戦闘能力が大幅に低下しますが、代わりに魔法関係の能力が上昇し魔法を一領域習得可能になります。",
	//華扇「奸佞邪智の鬼　」
	"あなたは千年間封印されていた鬼の腕との融合を果たしました。	この性格では種族が「鬼」に変化して身体能力が大幅に上昇し、倒した敵から体力を吸収するようになり、さらに特技が非常に攻撃的なものに変更されます。また習得できる魔法領域が妖術に変わります。",
	//光の三妖精「妖精大集合の」
	"チルノ、クラウンピース、エタニティラルバ、他数人の大妖精があなたたちに同行して一緒に戦うことになりました。これにより隠密以外の全パラメータが上昇し、装備の自由度がさらに上がり、格闘時の攻撃が強化され、使える特技が大幅に変化します。ただしスコアは1/3になります。",
	//お燐「探偵助手」
	"あなたは主のさとりの命令で探偵助手として調査の任務に出かけました。知能、器用、隠密能力、探索能力が大幅に上昇し情報収集に向いたいくつかの特技を習得します。ただし愛用の猫車は地霊殿に置いて来たので使用できません。",
	//文「社会派ルポライター」
	"あなたは異変調査のためにしばし素性を隠して活動することにしました。通常と違い頭に「頭襟」を装備する必要がありません。探索能力がやや上昇しますが肉弾戦関係の能力が少し低下します。",
	//女苑「石油王」
	"あなたと姉はひょんなことから石油を掘り当て石油王になりました。普段のように金を奪ったり散財したりする代わりに、石油を撒き散らしたり撒いた石油を燃やしたり姉を盾にしたりしながら戦います。いかにも富豪らしい雰囲気を醸し出すあなたは、街の店でいくらでもツケ払いで買い物をすることができます。しかし幻想郷に石油を換金する手段はなく、いずれ店主達はあなたからツケを取り立てることでしょう。",
	//魔理沙「闇市場調査員」
	"あなたはアビリティカードの闇市場の調査を始めました。この性格では通常の特技の代わりに職業「カード売人」と同じ特技を使用できます。スカートの隠しポケットにはいつもの自作の魔法の代わりにアビリティカードを8種類まで格納できます。魔法書を使った通常の魔法はこれまで通りに習得できます。",
	//龍「通貨発行者」
	"あなたはアビリティカードブームの仕掛け人で、カード交換用の通貨を発行して巨万の富を手にしました。このセーブデータで過去のプレイヤーがカード販売所に支払った総額の1/4を持ってゲームがスタートします。ただしこの資金は天狗社会の維持発展のために使われなければなりません。そのため街で一部の施設が使用不可能になります。ゲームのスコアも通常の1/4になります。",
	//慧ノ子「取り外せた」
	"あなたの両手のトラバサミは取り外すことができました。トラバサミを取り外すと両手の装備制限が無くなりますが、代わりに一部の特技と専用の強力な格闘攻撃が使えなくなります。この性格を選択するとスコアが25%上昇します。",


};

//特殊性格追加memo:
//#define SEIKAKU_SPECIAL_***を追加
//get_special_seikaku_index()に条件追加
//seikaku_special_jouhou[]に説明文追加
//table.cのseikaku_info_special[]にパラメータ追加
//スコアサーバスクリプトのhtmlout.pyに追記
//is_special_seikaku(SEIKAKU_SPECIAL_***)で判定してその性格に関する特殊処理

//クラスIDから,このクラスが特殊性格になったときの特殊性格インデックスを返す。
//race_idも使おうかと思ったが種族変容したらエラーになるのに気づいてボツ。
//この関数は実際のp_ptr->pseikakuに関わらず特殊性格インデックスを返す。
static int get_special_seikaku_index(int class_idx)
{

	if (class_idx == CLASS_CIRNO)		return SEIKAKU_SPECIAL_CIRNO;	
	if (class_idx == CLASS_PATCHOULI)	return SEIKAKU_SPECIAL_PATCHOULI;
	if (class_idx == CLASS_NARUMI)		return SEIKAKU_SPECIAL_NARUMI;
	if (class_idx == CLASS_UDONGE)		return SEIKAKU_SPECIAL_UDONGE;
	if (class_idx == CLASS_SUMIREKO)	return SEIKAKU_SPECIAL_SUMIREKO;
	if (class_idx == CLASS_SHINMYOU_2)	return SEIKAKU_SPECIAL_SHINMYOUMARU;
	if (class_idx == CLASS_BANKI)		return SEIKAKU_SPECIAL_SEKIBANKI;
	if (class_idx == CLASS_KASEN)		return SEIKAKU_SPECIAL_KASEN;
	if (class_idx == CLASS_3_FAIRIES)	return SEIKAKU_SPECIAL_3_FAIRIES;
	if (class_idx == CLASS_ORIN)		return SEIKAKU_SPECIAL_ORIN;
	if (class_idx == CLASS_AYA)			return SEIKAKU_SPECIAL_AYA;
	if (class_idx == CLASS_JYOON)		return SEIKAKU_SPECIAL_JYOON;
	if (class_idx == CLASS_MARISA)		return SEIKAKU_SPECIAL_MARISA;
	if (class_idx == CLASS_MEGUMU)		return SEIKAKU_SPECIAL_MEGUMU;
	if (class_idx == CLASS_ENOKO)		return SEIKAKU_SPECIAL_ENOKO;


	return SEIKAKU_SPECIAL_NONE;
}





/*:::魔法（と剣術）の領域説明文　添字はTV_BOOK_????に対応*/
static cptr realm_jouhou[TV_BOOK_END + 1] = {
	"",//ダミー
	"万物を構成するエレメントを操る領域です。酸・電撃・火炎・冷気の攻撃魔法が多く、加えて若干の防御・補助魔法が含まれます。属性の偏った敵に対して有効な打撃を与えられます。",
	"隠された物、知りえないはずの事を知覚する領域です。感知手段がほぼ完璧に揃い、敵の精神に攻撃する魔法もあります。強力な補助や回復もありますが、極めて高難度です。",
	"物体や自分の体に対し魔力を込めて特別な効果を与える領域です。防御と補助に極めて優れ、ある程度の戦闘手段もあります。いくつかの魔法はMPのほかに触媒が必要となります。",
	"何もないところに門を開き様々なものを呼び出す領域です。召喚とテレポート系に極めて優れますが、それ以外は不得手です。召喚の種類によってははMPのほかに代償が必要となることがあります。また、召喚魔法に失敗した時は召還しようとしたものが襲いかかってくることがあります。",
	"超自然的な奇跡を起こす領域です。感知と補助が充実しており様々な便利な魔法が揃いますが、敵に直接攻撃する魔法はありません。",
	"生命のエネルギーを操る領域です。回復に最も優れ、防御や補助も強力です。攻撃魔法もありますがアンデッドにのみ有効です。",
	"邪悪なるものを打ち倒す領域です。妖怪、悪魔、アンデッドに対して有効な攻撃魔法が揃い、補助魔法も充実しています。最上位の魔法書には強力な神降ろしの魔法がありますが、これらは二つ以上同時に効果を発揮することができません。",
	"自然の力を借りる領域です。多彩な魔法がバランス良く揃い、特に感知と防御に優れます。ただし攻撃魔法の威力はやや低めです。",
	"自らの体や周囲の地形を変化させる領域です。強力かつ多様な魔法が揃いますが詠唱に要するコストはやや高めです。また変身魔法の多くは防具が効果を発揮しなくなったり巻物が使えなくなるなど独特の制約を伴います。",
	"邪悪な力を操る領域です。毒や暗黒属性を中心とした攻撃魔法、極めて攻撃的な補助魔法、そして悪魔の使役や悪魔への変身といった呪文があります。しかし防御や感知はあまり得意ではありません。この領域の魔法を習得すると破邪攻撃に弱くなります。",
	"悪霊を使役したり呪いで攻撃する領域です。補助や召喚を中心に多彩な呪文が揃いますが、呪いによる攻撃は悪魔やアンデッドや無生物にはほぼ無力です。この領域の魔法を習得すると破邪攻撃に弱くなります。",
	"原初の無秩序なエネルギーを我が物とする領域です。相手を選ばない強力な攻撃魔法が揃います。いくつか補助魔法もありますが、それらも無秩序かつ破壊的なものです。この領域の全ての魔法の使用にはリスクが伴います。",
	"必殺剣とは剣術家たちの間で語り継がれ磨かれた剣の技です。武器での攻撃に独特の効果を上乗せするような技がほとんどです。必殺剣は一度習得すれば秘伝書を持っていなくても使用できます。ただし一部の技は特定の種類の武器を装備していないと使用できません。",
	"(調剤　現時点ではここで選択することはない)",
	"幻想郷で様々な噂や都市伝説が具現化するようになりました。時流に敏い一部の者達は外界から流れてきた書物を研究し、これら「オカルト」を魔法のように習得して使いこなします。オカルトは攻撃を中心に強力で多彩なものが揃いますが、特定の条件を満たしたり代償を支払わないと使えない癖の強いものがほとんどです。",
	"何者かの妖気が凝縮された「異変石」と呼ばれる魔石が発見されました。この石を研究することでこの地で起こった数々の異変を象徴する力を行使できます。石は8種類存在し一種類ごとに4つの魔法を習得可能で、攻撃を中心とした強力な魔法がバランス良く揃います。ただし序盤に欲しい石を狙って入手するのは難しく、また持ち物が異変石で一杯になりがちです。",
};



/*
static cptr realm_jouhou[VALID_REALM] =
{
#ifdef JP
"生命は回復能力に優れた魔法です。治療や防御、感知魔法が多く含まれていますが、攻撃呪文もわずかに持っています。特に高レベルの呪文にはアンデッドを塵に帰す力をあると言われています。",

"仙術は「meta」領域であり、感知や鑑定、さらに退却用の呪文や自身の能力を高める呪文などの便利な呪文が含まれています。しかし、直接攻撃用の呪文は持っていません。",

"自然の魔法は使用者を元素のマスターにします。これには防御、探知、治療と攻撃呪文が含まれています。また、生命以外の領域で最高の治療呪文もこの領域にあります。",

"カオスの魔法は制御が困難で、予測のできない魔法もあります。カオスは非常に非元素的であり、カオスの呪文は想像できる最も恐るべき破壊兵器です。この呪文を唱えるものはカオスの尖兵に対し、敵や自分自身さえも変異させるよう要求します。",

"黒魔術である暗黒の魔法ほど邪悪なカテゴリーはありません。これらの呪文は比較的学ぶのが困難ですが、高レベルになると術者に生物とアンデッドを自由に操る能力を与えます。残念なことに、もっとも強力な呪文はその触媒として術者自身の血を必要とし、詠唱中にしばしば術者を傷つけます。",

"トランプの魔法はテレポート系の呪文で精選されたものを持っており、その出入り口は他の生物を召喚するためにも使えるため、召喚呪文から選りすぐられたものも同様に持っています。しかし、この魔法によって全ての怪物が別の場所へ呼ばれるのを理解するわけではなく、もし召喚呪文に失敗するとその生物は敵となります。",

"秘術の魔法は、全ての領域から有用な呪文だけを取り入れようとした多用途領域です。必要な「道具」的呪文を持っていても高レベルの強力な呪文は持っていません。結果として、全ての呪文書は街で買い求めることができます。また、他の領域に存在する同様な呪文の方がより低レベル、低コストで唱えることができます。",

"匠の魔法は、自分や道具を強化するための魔法が含まれています。魔法によって自分自身の戦闘力を非常に高めることができますが、相手を直接攻撃するような呪文は含まれていません。",

"悪魔の魔法は暗黒と同様非常に邪悪なカテゴリーです。様々な攻撃魔法に優れ、また悪魔のごとき知覚能力を得ることができます。高レベルの呪文は悪魔を自在に操り、自分自身の肉体をも悪魔化させることができます。",

"破邪は「正義」の魔法です。直接敵を傷つける魔法が多く含まれ、特に邪悪な敵に対する力は恐るべきものがあります。しかし、善良な敵にはあまり効果がありません。",

"歌集は、歌によって効果を発揮する魔法です。魔法と同様、使った時に効果のあるものと、歌い続けることによって持続して効果を発揮するものがあります。後者の場合は、MPの続く限り効果を発揮することができますが、同時に歌える歌は1つだけという制限もあります。",

"武芸の書は、様々な戦闘の技について書かれています。この本は技を覚えるときに読む必要がありますが、一度覚えた技は使うのに本を持つ必要はありません。技を使うときには必ず武器を装備していなければいけません。",

"呪術は忌むべき領域です。複数の呪いの言葉を歌のように紡ぎながら詠唱します。多くの呪文は詠唱し続けることによって効果が持続されます。呪文には相手の行動を束縛するもの、ダメージを与えるもの、攻撃に対して反撃するものが多くあります。"
#else

"Life magic is very good for healing; it relies mostly on healing, protection and detection spells.  Also life magic have a few attack spells as well.  It said that some high level spell of life magic can disintegrate Undead monsters into ash.",

"Sorcery is a `meta` realm, including enchantment and general spells.  It provides superb protection spells, spells to enhance your odds in combat and, most importantly, a vast selection of spells for gathering information.  However, Sorcery has one weakness: it has no spells to deal direct damage to your enemies.",

"Nature magic makes you master of elements; it provides protection, detection, curing and attack spells.  Nature also has a spell of Herbal Healing, which is the only powerful healing spell outside the realm of Life magic.",

"There are few types of magic more unpredictable and difficult to control than Chaos magic.  Chaos is the very element of unmaking, and the Chaos spells are the most terrible weapons of destruction imaginable.  The caster can also call on the primal forces of Chaos to induce mutations in his/her opponents and even him/herself.",

"There is no fouler nor more evil category of spells than the necromantic spells of Death Magic.  These spells are relatively hard to learn, but at higher levels the spells give the caster power over living and the (un)dead, but the most powerful spells need his / her own blood as the focus, often hurting the caster in the process of casting.",

"Trump magic has, indeed, an admirable selection of teleportation spells.  Since the Trump gateways can also be used to summon other creatures, Trump magic has an equally impressive selection of summoning spells.  However, not all monsters appreciate being drawn to another place by Trump user.",

"Arcane magic is a general purpose realm of magic.  It attempts to encompass all 'useful' spells from all realms.  This is the downside of Arcane magic: while Arcane does have all the necessary 'tool' spells for a dungeon delver, it has no ultra-powerful high level spells.  As a consequence, all Arcane spellbooks can be bought in town.  It should also be noted that the 'specialized' realms usually offer the same spell at a lower level and cost. ",

"Craft magic can strengthen the caster or the equipments.  These spells can greatly improve the caster's fighting ability.  Using them against opponents directly is not possible.",

"Demon is a very evil realm, same as Death.  It provides various attack spells and devilish detection spells.  at higher levels, Demon magic provides ability to dominate demons, and to polymorph yourself into a demon.",

"Crusade is a magic of 'Justice'.  It includes damage spells, which are greatly effective against foul and evil monsters, but have poor effects against good monsters.",

"Music magic shows various effects as sing song.  There is two type of song; the one which shows effects instantly and the other one shows effect continuously until SP runs out.  But the latter type has a limit; only one song can be sing at the same time.",

"The books of Kendo describe about various combat techniques.  When learning new techniques, you are required to carry the books, but once you memorizes them, you don't have to carry them.  When using a technique, wielding a weapon is required.",

"Hex is a very terrible realm. Spells gives continual effects when they are spelled continually like songs. Spells may obstract monsters' actions, may deal damages in sight, may revenge against enemies."
#endif
};
*/
/*:::魔法領域選択時の補助説明　削除した*/
/*
///realm
static cptr realm_subinfo[VALID_REALM] =
{
#ifdef JP
"感知と防御と回復に優れています",
"攻撃はできませんが非常に便利です",
"感知と防御に優れています",
"破壊的な攻撃に優れています",
"生命のある敵への攻撃に優れています",
"召喚とテレポートに優れています",
"やや弱いながらも非常に便利です",
"直接戦闘の補助に優れています",
"攻撃と防御の両面に優れています",
"邪悪な怪物に対する攻撃に優れています",
"様々な魔法効果を持った歌を歌います",
"打撃攻撃に特殊能力を付加します",
"敵を邪魔しつつ復讐を狙います"
#else
"Good at detection and healing.",
"Utility and protective spells.",
"Good at detection and defence.",
"Offensive and destructive.",
"Ruins living creatures.",
"Good at summoning, teleportation.",
"Very useful but poor a bit.",
"Support for melee fighting.",
"Good at both offence and defence.",
"Destroys evil creatures.",
"Song with magical effects.",
"Special attacks on melee.",
"Good at obstacle and revenge."
#endif
};
*/

/*
 * Autoroll limit
 */
 /*:::ここでしか使われてない。オートローラーの最低値か?*/
static s16b stat_limit[6];

/*:::体格オートローラー用か*/
static struct {
	s16b agemin, agemax;
	s16b htmin, htmax;
	s16b wtmin, wtmax;
	s16b scmin, scmax;
} chara_limit;

/*
 * Autoroll matches
 */
static s32b stat_match[6];

/*
 * Autoroll round
 */
static s32b auto_round;


static void birth_quit(void)
{
	remove_loc();
	quit(NULL);
}


/*
 *  Show specific help file
 */
static void show_help(cptr helpfile)
{
	/* Save screen */
	screen_save();

	/* Peruse the help file */
	(void)show_file(TRUE, helpfile, NULL, 0, 0);

	/* Load screen */
	screen_load();
}

/*:::魔法の領域名、適性を表示*/
///mod160620 オカルト追加
void print_realm_aptitude(int realm, bool second)
{

	char buf[80];
	cptr category[4] ={"魔術","仙術","妖術"};

	//if(realm < MIN_MAGIC || realm > MAX_BASIC_MAGIC_REALM && realm != TV_BOOK_OCCULT) return;
	if(realm < MIN_MAGIC || realm > TV_BOOK_END) return;

	put_str("                                   ", 3, 40);
	put_str("                                   ", 4, 40);
	put_str("                                   ", 5, 40);
	put_str("                                   ", 6, 40);

	if(realm <= MAX_BASIC_MAGIC_REALM)
		sprintf(buf, "%sの%s", realm_names[realm], category[(realm - 1) / 4]);
	else
		sprintf(buf, "%sの秘術", realm_names[realm]);

	c_put_str(TERM_L_BLUE, buf, 3, 40);
	sprintf(buf, "職業適性：%c", 'G' - cp_ptr->realm_aptitude[realm]);
	c_put_str(TERM_WHITE, buf, 4, 40);
	c_put_str(TERM_WHITE, "種族適性：", 5, 40);
	switch(rp_ptr->realm_aptitude[realm])
	{
	case 0:
		c_put_str(TERM_L_DARK, "無理", 5, 50);break;
	case 1:case 2:
		c_put_str(TERM_L_RED, "嫌い", 5, 50);break;
	case 3:case 4:
		c_put_str(TERM_ORANGE, "苦手", 5, 50);break;
	case 5:
		c_put_str(TERM_WHITE, "普通", 5, 50);break;
	case 6:case 7:
		c_put_str(TERM_YELLOW, "得意", 5, 50);break;
	case 8:case 9:
		c_put_str(TERM_L_GREEN, "卓越", 5, 50);break;
	default:
		c_put_str(TERM_L_RED, "ERROR:指定外の数値", 5, 50);break;
	}
	if(second && !same_category_realm(p_ptr->realm1,realm))
		c_put_str(TERM_L_UMBER, "(注：第一領域と相性が悪い領域)", 6, 40);

}


/*
 * Choose from one of the available magical realms
 */
static byte choose_realm(int *count)
{
	int picks[TV_BOOK_END] = {0};
	int k, i, cs, os;
	byte auto_select = REALM_NONE;
	int n = 0;
	char c;
	char sym[TV_BOOK_END];
	char p2 = ')';
	char buf[80], cur[80];

	if(p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
	{
		*count = 1;
		return TV_BOOK_HISSATSU;
	}
	//魔法習得不可/不要な職は終了
	else if(cp_ptr->realm_aptitude[0] == 0 || REALM_SPELLMASTER)
	{
		*count = 0;
		return REALM_NONE;
	}
	//v1.1.60 赤蛮奇は専用性格「スマホ首」以外魔法習得できない
	//この関数は普通性格選択前に呼ばれるが、赤蛮奇が性格「スマホ首」にしたときのときのみもう一度呼ばれる
	else if (p_ptr->pclass == CLASS_BANKI && !is_special_seikaku(SEIKAKU_SPECIAL_SEKIBANKI))
	{
		*count = 0;
		return REALM_NONE;

	}

	for(i=1;i<=TV_BOOK_END;i++)
	{
		if(cp_ptr->realm_aptitude[i] > 1)
		{

			//v2.1.0 瑞霊は第一領域に仙術、第二領域に妖術のみ選択可能
			if (p_ptr->pclass == CLASS_MIZUCHI)
			{
				if (p_ptr->realm1 == REALM_NONE)
				{
					if (i >= TV_BOOK_TRANSFORM) continue;
				}
				else
				{
					if (i <  TV_BOOK_TRANSFORM) continue;
				}
			}


			//v1.1.65 華扇は特殊性格にすると妖術選択可能になる(性格選択後に再びこの関数に戻ってくる)
			if (p_ptr->pclass == CLASS_KASEN)
			{
				if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
				{
					if (i > TV_BOOK_SUMMONS && i < TV_BOOK_TRANSFORM) continue;
				}
				else
				{
					if (i >= TV_BOOK_TRANSFORM) continue;
				}
			}


			if(p_ptr->realm1 != REALM_NONE)
			{
				if(p_ptr->realm1 == i) continue;
				if(p_ptr->pclass == CLASS_PRIEST && !same_category_realm(p_ptr->realm1,i)) continue;
			}
			picks[(*count)] = i;
			(*count)++;
			auto_select = i;
		}
	}
#if 0
	/* Count the choices */
	if (choices & CH_LIFE)
	{
		(*count)++;
		auto_select = REALM_LIFE;
	}
	if (choices & CH_SORCERY)
	{
		(*count)++;
		auto_select = REALM_SORCERY;
	}
	if (choices & CH_NATURE)
	{
		(*count)++;
		auto_select = REALM_NATURE;
	}
	if (choices & CH_CHAOS)
	{
		(*count)++;
		auto_select = REALM_CHAOS;
	}
	if (choices & CH_DEATH)
	{
		(*count)++;
		auto_select = REALM_DEATH;
	}
	if (choices & CH_TRUMP)
	{
		(*count)++;
		auto_select = REALM_TRUMP;
	}
	if (choices & CH_ARCANE)
	{
		(*count)++;
		auto_select = REALM_ARCANE;
	}
	if (choices & CH_ENCHANT)
	{
		(*count)++;
		auto_select = REALM_CRAFT;
	}
	if (choices & CH_DAEMON)
	{
		(*count)++;
		auto_select = REALM_DAEMON;
	}
	if (choices & CH_CRUSADE)
	{
		(*count)++;
		auto_select = REALM_CRUSADE;
	}
	if (choices & CH_MUSIC)
	{
		(*count)++;
		auto_select = REALM_MUSIC;
	}
	if (choices & CH_HISSATSU)
	{
		(*count)++;
		auto_select = REALM_HISSATSU;
	}
	if (choices & CH_HEX)
	{
		(*count)++;
		auto_select = REALM_HEX;
	}
#endif
	clear_from(10);

	/* Auto-select the realm */
	/*:::習得可能な領域が1つか非魔法職ならここで終了*/
	if ((*count) < 2) return auto_select;

	///class
	/* Constraint to the 1st realm */
/*
	if (p_ptr->realm2 != 255)
	{
		if (p_ptr->pclass == CLASS_PRIEST)
		{
			if (is_good_realm(p_ptr->realm1))
			{
				choices &= ~(CH_DEATH | CH_DAEMON);
			}
			else
			{
				choices &= ~(CH_LIFE | CH_CRUSADE);
			}
		}
	}
*/
	/* Extra info */
#ifdef JP
	if(p_ptr->pclass == CLASS_LYRICA || p_ptr->pclass == CLASS_MERLIN)
		put_str ("(ここで選択した魔法はルナサにしか使えません)", 11, 5);

	put_str ("注意：魔法の領域の選択によりあなたが習得する呪文のタイプが決まります。", 23, 5);
#else
	put_str ("Note: The realm of magic will determine which spells you can learn.", 23, 5);
#endif

	cs = 0;
	for(i=1;i<=(*count);i++)
	{
		sym[n] = I2A(n);
		sprintf(buf, "%c%c %s", sym[n], p2, realm_names[picks[n]]);
		put_str(buf, 12 + (n/4), 2 + 15 * (n%4));
		n++;
	}
#if 0
	cs = 0;
	for (i = 0; i<32; i++)
	{
		/* Analize realms */
		if (choices & (1L << i))
		{
			if (p_ptr->realm1 == i+1)
			{
				
				if (p_ptr->realm2 == 255)
					cs = n;
				/*:::第二領域選択時、第一領域と同じ領域が表示されなくなる処理らしい*/
				else
					continue;
			}
			if (p_ptr->realm2 == i+1)
				cs = n;

			sym[n] = I2A(n);

			sprintf(buf, "%c%c %s", sym[n], p2, realm_names[i+1]);
			put_str(buf, 12 + (n/5), 2 + 15 * (n%5));
			picks[n++] = i+1;
		}
	}
#endif
#ifdef JP
	sprintf(cur, "%c%c %s", '*', p2, "ランダム");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Get a realm */
	k = -1;
	os = n;
	while (1)	
	{
		/* Move Cursol */
		if (cs != os)
		{
			c_put_str(TERM_WHITE, cur, 12 + (os/4), 2 + 15 * (os%4));
			put_str("                                   ", 3, 40);
			put_str("                                   ", 4, 40);
			put_str("                                   ", 5, 40);
			put_str("                                   ", 6, 40);

			if(cs == n)
			{
#ifdef JP
				sprintf(cur, "%c%c %s", '*', p2, "ランダム");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
			}
			else
			{
				sprintf(cur, "%c%c %s", sym[cs], p2, realm_names[picks[cs]]);
				print_realm_aptitude(picks[cs],(p_ptr->realm1 != REALM_NONE));

			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/4), 2 + 15 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "領域を選んで下さい(%c-%c) ('='初期オプション設定): ", sym[0], sym[n-1]);
#else
		sprintf(buf, "Choose a realm (%c-%c) ('=' for options): ", sym[0], sym[n-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return 255;
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == n)
			{
				k = randint0(n);
				break;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			k = randint0(n);
			break;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < n) cs++;
		}
		if (c == '2')
		{
			if ((cs + 4) <= n) cs += 4;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < n))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < n))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("tmagic.txt#Realm");
#else
			show_help("magic.txt#MagicRealms");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth option((*)s effect score)");
#endif

			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Clean up */
	clear_from(10);

	return (picks[k]);
}

/*
 * Choose the magical realms
 */
static bool get_player_realms(void)
{
	int i, count;
	char buf[80];

	/* Clean up infomation of modifications */
	put_str("                                           ", 3, 40);
	put_str("                                           ", 4, 40);
	put_str("                                           ", 5, 40);
	put_str("                                           ", 6, 40);

	/* Select the first realm */
	while (1)
	{
		char temp[80*10];
		cptr t;
		count = 0;

		p_ptr->realm1 = REALM_NONE;
		p_ptr->realm2 = 255;
		p_ptr->realm1 = choose_realm(&count);

		if (255 == p_ptr->realm1) return FALSE;
		if (!p_ptr->realm1) break;

		/* Clean up*/
		clear_from(10);
		print_realm_aptitude(p_ptr->realm1,FALSE);

		roff_to_buf(realm_jouhou[p_ptr->realm1], 74, temp, sizeof(temp));
		t = temp;
		for (i = 0; i < 10; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}

		if (count < 2)
		{
#ifdef JP
			prt("何かキーを押してください", 0, 0);
#else
			prt("Hit any key.", 0, 0);
#endif
			(void)inkey();
			prt("", 0, 0);
			break;
		}
		else if(rp_ptr->realm_aptitude[p_ptr->realm1] == 0)
		{
			msg_format("あなたにはその領域を修得することはできそうにない。");
		}
		if (get_check_strict("よろしいですか？", CHECK_DEFAULT_Y)) break;
	}
	if(p_ptr->realm1 != REALM_NONE)
	{
		put_str("魔法        :", 6, 1);
		c_put_str(TERM_L_BLUE, realm_names[p_ptr->realm1], 6, 15);
	}
	/* Select the second realm */
	p_ptr->realm2 = REALM_NONE;
	if (p_ptr->realm1 && cp_ptr->realm_aptitude[0] == 3)
	{

		/* Select the second realm */
		while (1)
		{
			char temp[80*8];
			cptr t;

			p_ptr->realm2 = REALM_NONE;
			count = 0;
			p_ptr->realm2 = choose_realm(&count);

			if (255 == p_ptr->realm2) return FALSE;
			if (!p_ptr->realm2) break;

			/* Clean up*/
			clear_from(10);
			print_realm_aptitude(p_ptr->realm2,TRUE);
			roff_to_buf(realm_jouhou[p_ptr->realm2], 74, temp, sizeof(temp));
			t = temp;
			for (i = 0; i< 6; i++)
			{
				if(t[0] == 0)
					break; 
				else
				{
					prt(t, 12+i, 3);
					t += strlen(t) + 1;
				}
			}

			if (count < 2)
			{
#ifdef JP
				prt("何かキーを押してください", 0, 0);
#else
				prt("Hit any key.", 0, 0);
#endif
				(void)inkey();
				prt("", 0, 0);
				break;
			}
#ifdef JP


			if(rp_ptr->realm_aptitude[p_ptr->realm2] == 0)
			{
				msg_format("あなたにはその領域を修得することはできそうにない。");
			}
			if (get_check_strict("よろしいですか？", CHECK_DEFAULT_Y)) break;
#else
			else if (get_check_strict("Are you sure? ", CHECK_DEFAULT_Y)) break;
#endif

		}
		clear_from(10);

		if (p_ptr->realm2)
		{
			/* Print the realm */
			c_put_str(TERM_L_BLUE, format("%s, %s", realm_names[p_ptr->realm1], realm_names[p_ptr->realm2]), 6, 15);
		}
	}
	if(p_ptr->pclass == CLASS_MAGE)
	{
		if(p_ptr->realm1 <= TV_BOOK_SUMMONS) strcpy(player_class_name, "魔術師");
		else if(p_ptr->realm1 <= TV_BOOK_NATURE) strcpy(player_class_name, "道師");
		else if(p_ptr->realm1 <= TV_BOOK_CHAOS) strcpy(player_class_name, "妖術師");
		else  strcpy(player_class_name, "秘術師");

	}
	else if( p_ptr->pclass == CLASS_HIGH_MAGE)
	{
		if(p_ptr->realm1 == TV_BOOK_ELEMENT) strcpy(player_class_name, "エーテルマスター");
		else if(p_ptr->realm1 == TV_BOOK_FORESEE) strcpy(player_class_name, "プロフェット");
		else if(p_ptr->realm1 == TV_BOOK_ENCHANT) strcpy(player_class_name, "エンチャンター");
		else if(p_ptr->realm1 == TV_BOOK_SUMMONS) strcpy(player_class_name, "サマナー");
		else if(p_ptr->realm1 == TV_BOOK_MYSTIC) strcpy(player_class_name, "ミスティック");
		else if(p_ptr->realm1 == TV_BOOK_LIFE) strcpy(player_class_name, "リジェネレーター");
		else if(p_ptr->realm1 == TV_BOOK_PUNISH) strcpy(player_class_name, "シャーマン");
		else if(p_ptr->realm1 == TV_BOOK_NATURE) strcpy(player_class_name, "ジオマンサー");
		else if(p_ptr->realm1 == TV_BOOK_TRANSFORM) strcpy(player_class_name, "シェイプシフター");
		else if(p_ptr->realm1 == TV_BOOK_DARKNESS) strcpy(player_class_name, "デビルサマナー");
		else if(p_ptr->realm1 == TV_BOOK_NECROMANCY) strcpy(player_class_name, "ネクロマンサー");
		else if(p_ptr->realm1 == TV_BOOK_CHAOS) strcpy(player_class_name, "ログルスマスター");
		else if (p_ptr->realm1 == TV_BOOK_OCCULT) strcpy(player_class_name, "オカルティスト");
		else strcpy(player_class_name, "ジェモロジスト");

	}
		c_put_str(TERM_WHITE, "                 ", 5, 15);
		c_put_str(TERM_L_BLUE, player_class_name, 5, 15);



	return (TRUE);
}

/*
 * Save the current data for later
 */
/*:::キャラメイク情報の記録*/
/*:::ユニークプレイヤー実装後も使い回しは可能そうだが、stat_MAX_MAXは変更予定*/
static void save_prev_data(birther *birther_ptr)
{
	int i;

	/* Save the data */
	birther_ptr->psex = p_ptr->psex;
	birther_ptr->prace = p_ptr->prace;
	birther_ptr->pclass = p_ptr->pclass;
	birther_ptr->pseikaku = p_ptr->pseikaku;
	birther_ptr->realm1 = p_ptr->realm1;
	birther_ptr->realm2 = p_ptr->realm2;
	birther_ptr->age = p_ptr->age;
	birther_ptr->ht = p_ptr->ht;
	birther_ptr->wt = p_ptr->wt;
	birther_ptr->sc = p_ptr->sc;
	birther_ptr->au = p_ptr->au;
	///mod140307 難易度変更
	birther_ptr->difficulty = difficulty;


	/* Save the stats */
	for (i = 0; i < 6; i++)
	{
		birther_ptr->stat_max[i] = p_ptr->stat_max[i];
		birther_ptr->stat_max_max[i] = p_ptr->stat_max_max[i];
	}

	/* Save the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		birther_ptr->player_hp[i] = p_ptr->player_hp[i];
	}

	birther_ptr->chaos_patron = p_ptr->chaos_patron;

	///sysdel
	/* Save the virtues */
	for (i = 0; i < 8; i++)
	{
		birther_ptr->vir_types[i] = p_ptr->vir_types[i];
	}

	/* Save the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(birther_ptr->history[i], p_ptr->history[i]);
	}
}


/*
 * Load the previous data
 */
/*:::クイックスタート用に以前のキャラメイク情報を得る*/
///system 大がかりなパラメータ変更をしたらここも変更必要か
static void load_prev_data(bool swap)
{
	int i;

	birther	temp;

	/*** Save the current data ***/
	if (swap) save_prev_data(&temp);


	/*** Load the previous data ***/

	/* Load the data */
	p_ptr->psex = previous_char.psex;
	p_ptr->prace = previous_char.prace;
	p_ptr->pclass = previous_char.pclass;
	p_ptr->pseikaku = previous_char.pseikaku;
	p_ptr->realm1 = previous_char.realm1;
	p_ptr->realm2 = previous_char.realm2;
	p_ptr->age = previous_char.age;
	p_ptr->ht = previous_char.ht;
	p_ptr->wt = previous_char.wt;
	p_ptr->sc = previous_char.sc;
	p_ptr->au = previous_char.au;

	///mod140307 難易度変更
	difficulty = previous_char.difficulty;

	/* Load the stats */
	for (i = 0; i < 6; i++)
	{
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = previous_char.stat_max[i];
		p_ptr->stat_max_max[i] = previous_char.stat_max_max[i];
	}

	/* Load the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		p_ptr->player_hp[i] = previous_char.player_hp[i];
	}
	p_ptr->mhp = p_ptr->player_hp[0];
	p_ptr->chp = p_ptr->player_hp[0];

	p_ptr->chaos_patron = previous_char.chaos_patron;

	for (i = 0; i < 8; i++)
	{
		p_ptr->vir_types[i] = previous_char.vir_types[i];
	}

	/* Load the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(p_ptr->history[i], previous_char.history[i]);
	}


	/*** Save the previous data ***/
	if (swap)
	{
		(void)COPY(&previous_char, &temp, birther);
	}
}




/*
 * Returns adjusted stat -JK-  Algorithm by -JWT-
 */
///mod140104 パラメータ上限と表記を変更
static int adjust_stat(int value, int amount)
{
	int i;

	value += amount;
	if(value < 3) value = 3;
	return (value);




#if 0
	/* Negative amounts */
	if (amount < 0)
	{
		/* Apply penalty */
		for (i = 0; i < (0 - amount); i++)
		{
			if (value >= 18+10)
			{
				value -= 10;
			}
			else if (value > 18)
			{
				value = 18;
			}
			else if (value > 3)
			{
				value--;
			}
		}
	}

	/* Positive amounts */
	else if (amount > 0)
	{
		/* Apply reward */
		for (i = 0; i < amount; i++)
		{
			if (value < 18)
			{
				value++;
			}
			else
			{
				value += 10;
			}
		}
	}

	/* Return the result */
	return (value);
#endif
}




/*
 * Roll for a characters stats
 *
 * For efficiency, we include a chunk of "calc_bonuses()".
 */
/*:::ステータスをロールしてp_ptr->stat_cur[],stat_max[]に格納する*/
/*:::ステータスは8～17 パラメータ合計が73～86の範囲内*/
static void get_stats(void)
{
	/* Roll and verify some stats */
	while (TRUE)
	{
		int i;
		int sum = 0;

		/* Roll some dice */
		for (i = 0; i < 2; i++)
		{
			s32b tmp = randint0(60*60*60);
			int val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			/* Save that value */
			sum += val;
			p_ptr->stat_cur[3*i] = p_ptr->stat_max[3*i] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			/* Save that value */
			sum += val;
			p_ptr->stat_cur[3*i+1] = p_ptr->stat_max[3*i+1] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp;

			/* Save that value */
			sum += val;
			p_ptr->stat_cur[3*i+2] = p_ptr->stat_max[3*i+2] = val;
		}

		/* Verify totals */
		if ((sum > 42+5*6) && (sum < 57+5*6)) break;
		/* 57 was 54... I hate 'magic numbers' :< TY */
	}
}

/*:::基本ステータス上昇最大値(18/70-18/130)を得る*/
/*:::最大値はp_ptr->stat_max_max[]に格納され、今のパラメータがそれ以上なら現在パラメータが下げられる*/

///mod140104 パラメータ上限と表記を変更　最大値は常に30(旧18/120)になるようにした
void get_max_stats(void)
{
	int		i, j;

	int		dice[6];
#if 0
	/* Roll and verify some stats */
	/*:::1-7を得る。合計が24になるように。4のとき最大値は18/100になる。*/
	while (TRUE)
	{
		/* Roll some dice */
		for (j = i = 0; i < 6; i++)
		{
			/* Roll the dice */
			dice[i] = randint1(7);

			/* Collect the maximum */
			j += dice[i];
		}

		/* Verify totals */
		if (j == 24) break;
	}

	/* Acquire the stats */
	for (i = 0; i < 6; i++)
	{
		j = 18 + 60 + dice[i]*10;

		/* Save that value */
		p_ptr->stat_max_max[i] = j;
		if (p_ptr->stat_max[i] > j)
			p_ptr->stat_max[i] = j;
		if (p_ptr->stat_cur[i] > j)
			p_ptr->stat_cur[i] = j;
	}
#endif
	/*:::新しい最大値を適用。新生のときstat_maxが新しい最大値を超えることはもうないはず*/
	for(i=0;i<6;i++) p_ptr->stat_max_max[i] = STAT_MAX_MAX;

	p_ptr->knowledge &= ~(KNOW_STAT);

	/* Redisplay the stats later */
	p_ptr->redraw |= (PR_STATS);
}


/*
 * Roll for some info that the auto-roller ignores
 */
/*:::キャラメイクに関するいくつかの特殊事項を実行*/
///race class seikaku
///system キャラメイクに密接に関係
static void get_extra(bool roll_hitdie)
{
	int i, j;

	/* Experience factor */
	/*:::expペナルティを計算*/
	if (p_ptr->prace == RACE_ANDROID) p_ptr->expfact = rp_ptr->r_exp;
	else p_ptr->expfact = rp_ptr->r_exp + cp_ptr->c_exp;

	if(EXTRA_MODE && p_ptr->prace != RACE_ANDROID) p_ptr->expfact /= 2;

	//if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_NINJA)) && ((p_ptr->prace == RACE_KLACKON) || (p_ptr->prace == RACE_SPRITE)))
	//	p_ptr->expfact -= 15;

	/* Reset record of race/realm changes */
	/*:::「以前は～だった」的情報のクリア*/
	p_ptr->start_race = p_ptr->prace;
	p_ptr->old_race1 = 0L;
	p_ptr->old_race2 = 0L;
	p_ptr->old_realm = 0;

	/*:::スペマスと赤魔の経験値処理*/
	for (i = 0; i < 64; i++)
	{
		if (p_ptr->pclass == CLASS_SORCERER) p_ptr->spell_exp[i] = SPELL_EXP_MASTER;
		else if (p_ptr->pclass == CLASS_RED_MAGE) p_ptr->spell_exp[i] = SPELL_EXP_SKILLED;
		else p_ptr->spell_exp[i] = SPELL_EXP_UNSKILLED;
	}
	///skill 武器技能
/*
	for (i = 0; i < 5; i++)
		for (j = 0; j < 64; j++)
			p_ptr->weapon_exp[i][j] = s_info[p_ptr->pclass].w_start[i][j];
	if ((p_ptr->pseikaku == SEIKAKU_SEXY) && (p_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] < WEAPON_EXP_BEGINNER))
	{
		p_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_BEGINNER;
	}
*/
	///mod131226 skill 技能と武器技能の統合
	///mod140130 skill 技能と武器技能初期値設定
	//for (i = 0; i < 10; i++)
		//p_ptr->skill_exp[i] = s_info[p_ptr->pclass].s_start[i];

	for (i = 0; i < SKILL_EXP_MAX; i++)
	{
		switch(cp_ptr->skill_aptitude[i])
		{
		case 0: case 1: case 2:
			p_ptr->skill_exp[i] =  0;
			break;
		case 3:
			p_ptr->skill_exp[i] =  800;
			break;
		case 4:
			p_ptr->skill_exp[i] =  1600;
			break;
		case 5:
			p_ptr->skill_exp[i] =  2400;
			break;
		default:
			msg_format("ERROR:skill_aptitude[]が不正な値");
		}
		/*:::教師は最初から技能が高い*/
		if(p_ptr->pclass == CLASS_TEACHER) p_ptr->skill_exp[i] =  2400;
		/*:::星は戦いの心得がない*/
		if(p_ptr->pclass == CLASS_SHOU) p_ptr->skill_exp[i] =  0;
		//妹紅は技能が高い(銃除く)
		if(p_ptr->pclass == CLASS_MOKOU && i != 20) p_ptr->skill_exp[i] =  3200;

	}

	///mod150824 プリズムリバー三姉妹特殊処理　ハードコーディング多数　prism_change_class()参照
	if(CLASS_IS_PRISM_SISTERS)
	{
		//自宅で待機してる姉妹の分の初期技能値設定
		for(i=0;i<3;i++)
		{
			for (j = 0; j < SKILL_EXP_MAX; j++)
			{
				int init_skill = MAX(0,(class_info[CLASS_LUNASA+i].skill_aptitude[j]-2)) * 800;
				p_ptr->magic_num1[3 + j + SKILL_EXP_MAX * i] = init_skill;
			}
		}
		//ラッキーガールのとき全員に変異フラグ付与
		if(p_ptr->pseikaku == SEIKAKU_LUCKY)
		{
			p_ptr->magic_num2[14] = 0x80;
			p_ptr->magic_num2[26] = 0x80;
			p_ptr->magic_num2[38] = 0x80;
			p_ptr->magic_num2[50] = 0x80;
			//v1.1.15 66→62
			p_ptr->magic_num2[62] = 0x80;
			p_ptr->magic_num2[74] = 0x80;
		}
	}


	/* Hitdice */
	/*:::HD計算*/
	//v1.1.32 パチュリー特殊性格
	if (p_ptr->pclass == CLASS_SORCERER || is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
		p_ptr->hitdie = rp_ptr->r_mhp/2 + cp_ptr->c_mhp + ap_ptr->a_mhp;
	else
		p_ptr->hitdie = rp_ptr->r_mhp + cp_ptr->c_mhp + ap_ptr->a_mhp;

	/* Roll for hit point unless quick-start */
	/*:::HP計算*/
	if (roll_hitdie) do_cmd_rerate_aux();

	/* Initial hitpoints */
	p_ptr->mhp = p_ptr->player_hp[0];
}


/*
 * Get the racial history, and social class, using the "history charts".
 */
///sysdel race
/*:::生い立ちデフォルトと社会的地位を得る。*/
static void get_history(void)
{
	int i, n, chart, roll, social_class;

	char *s, *t;

	char buf[240];

	///tmp131221 社会的地位とデフォルト生い立ち文章を固定
	p_ptr->sc = 50;
	strcpy(p_ptr->history[0], "キャラクターの生い立ちや決意表明など好きに書いてください");
	return;
///del140105 ここから先に種族ごとの生い立ちデフォルト決定処理があったが種族変更が面倒なので全部消した


}


/*
 * Get character's height and weight
 */
void get_height_weight(void)
{
	int h_percent; /* 身長が平均にくらべてどのくらい違うか. */

	/* Calculate the height/weight for males */
	if (p_ptr->psex == SEX_MALE)
	{
		p_ptr->ht = randnor(rp_ptr->m_b_ht, rp_ptr->m_m_ht);
		h_percent = (int)(p_ptr->ht) * 100 / (int)(rp_ptr->m_b_ht);
		p_ptr->wt = randnor((int)(rp_ptr->m_b_wt) * h_percent /100
				    , (int)(rp_ptr->m_m_wt) * h_percent / 300 );
	}
  
	/* Calculate the height/weight for females */
	else if (p_ptr->psex == SEX_FEMALE)
	{
		p_ptr->ht = randnor(rp_ptr->f_b_ht, rp_ptr->f_m_ht);
		h_percent = (int)(p_ptr->ht) * 100 / (int)(rp_ptr->f_b_ht);
		p_ptr->wt = randnor((int)(rp_ptr->f_b_wt) * h_percent /100
				    , (int)(rp_ptr->f_m_wt) * h_percent / 300 );
	}
}


/*
 * Computes character's age, height, and weight
 * by henkma
 */
///sysdel
/*:::年齢、身長、体重を得てp_ptr->age,p_ptr->ht,p_ptr->wtに格納する。性別と種族が関係する。*/
static void get_ahw(void)
{
	/* Get character's age */
	//v1.1.70 動物霊の所属勢力決定用乱数の代わりとしてこれを使っている
	p_ptr->age = rp_ptr->b_age + randint1(rp_ptr->m_age);

	/* Get character's height and weight */
	get_height_weight();
}


/*
 * Get the player's starting money
 */
///class system seikaku
/*:::初期所持金　ステータスが高いと減るらしい*/
static void get_money(void)
{
	int i, gold;

	/* Social Class determines starting gold */
	gold = 1000 + randint1(1000);
	if (p_ptr->pclass == CLASS_CIVILIAN)  gold += gold*2 + randint0(2);
	else if (p_ptr->pclass == CLASS_RAN)  gold += gold*3 + randint0(3);


	while(one_in_(7)) gold *= 2;

	if(p_ptr->pclass == CLASS_SHOU) gold *= 3;

	///mod140104 パラメータ表記変更に伴い、高パラメータで所持金が減る処理をとりあえず消した
#if 0
	/* Process the stats */
	for (i = 0; i < 6; i++)
	{
		/* Mega-Hack -- reduce gold for high stats */
		if (p_ptr->stat_max[i] >= 18 + 50) gold -= 300;
		else if (p_ptr->stat_max[i] >= 18 + 20) gold -= 200;
		else if (p_ptr->stat_max[i] > 18) gold -= 150;
		else gold -= (p_ptr->stat_max[i] - 8) * 10;
	}
#endif
	/* Minimum 100 gold */
	//if (gold < 100) gold = 100;

	if(p_ptr->pclass == CLASS_OUTSIDER) gold = 0;
/*
	if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
		gold /= 2;
	else 
*/		
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
		gold = 10000000;
	//if (p_ptr->prace == RACE_ANDROID) gold /= 5;


	if(p_ptr->pclass == CLASS_TEWI) gold = 1000000;
	else if (p_ptr->pclass == CLASS_SHION) gold = -99999999;
	else if (is_special_seikaku(SEIKAKU_SPECIAL_MEGUMU)) gold = MAX(gold,(int)(total_pay_cardshop / 4));


	/* Save the gold */
	p_ptr->au = gold;
}



/*
 * Display stat values, subset of "put_stats()"
 *
 * See 'display_player()' for screen layout constraints.
 */
/*:::オートローラー中、定期的に現在のステータスを画面の決まった位置に表示する*/
static void birth_put_stats(void)
{
	int i, j, m, p;
	int col;
	byte attr;
	char buf[80];


	if (autoroller)
	{
		col = 42;
		/* Put the stats (and percents) */
		for (i = 0; i < 6; i++)
		{
			/* Race/Class bonus */
			j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i];

			/* Obtain the current stat */
			m = adjust_stat(p_ptr->stat_max[i], j);

			/* Put the stat */
			cnv_stat(m, buf);
			c_put_str(TERM_L_GREEN, buf, 3+i, col+24);

			/* Put the percent */
			if (stat_match[i])
			{
				if (stat_match[i] > 1000000L)
				{
					/* Prevent overflow */
					p = stat_match[i] / (auto_round / 1000L);
				}
				else
				{
					p = 1000L * stat_match[i] / auto_round;
				}
			
				attr = (p < 100) ? TERM_YELLOW : TERM_L_GREEN;
				sprintf(buf, "%3d.%d%%", p/10, p%10);
				c_put_str(attr, buf, 3+i, col+13);
			}

			/* Never happened */
			else
			{
#ifdef JP
				c_put_str(TERM_RED, "(なし)", 3+i, col+13);
#else
				c_put_str(TERM_RED, "(NONE)", 3+i, col+13);
#endif

			}
		}
	}
}


static void k_info_reset(void)
{
	int i;

	/* Reset the "objects" */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Reset "tried" */
		k_ptr->tried = FALSE;

		/* Reset "aware" */
		k_ptr->aware = FALSE;
	}
}


/*
 * Clear all the global "character" data
 */
/*:::＠の全情報初期化*/
//何かセーブデータに残るような変数を増やしたらここで忘れず初期化する
static void player_wipe(void)
{
	int i;

	/*:::メッセージ履歴クリア*/
	/* Hack -- free the "last message" string */
	if (p_ptr->last_message) string_free(p_ptr->last_message);

	/*:::p_ptrのクリア*/
	/* Hack -- zero the struct */
	(void)WIPE(p_ptr, player_type);

	/* Wipe the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(p_ptr->history[i], "");
	}

	/*:::クエストやアイテムなど＠にかかわるグローバル変数の初期化*/
	/* Wipe the quests */
	for (i = 0; i < max_quests; i++)
	{
		quest[i].status = QUEST_STATUS_UNTAKEN;

		quest[i].cur_num = 0;
		quest[i].max_num = 0;
		quest[i].type = 0;
		quest[i].level = 0;
		quest[i].r_idx = 0;
		quest[i].complev = 0;
		//v1.1.25
		quest[i].comptime = 0;
	}

	/* No weight */
	p_ptr->total_weight = 0;

	/* No items */
	inven_cnt = 0;
	equip_cnt = 0;

	/* Clear the inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&inventory[i]);
	}

	/* Clear the inventory2 */
	for (i = 0; i < INVEN_ADD_MAX; i++)
	{
		object_wipe(&inven_add[i]);
	}
	/* Clear the inventory3 */
	///mod160212 うっかりinven_addを初期化してた。修正
	for (i = 0; i < INVEN_SPECIAL_MAX; i++)
	{
		object_wipe(&inven_special[i]);
	}

	/* Start with no artifacts made yet */
	for (i = 0; i < max_a_idx; i++)
	{
		artifact_type *a_ptr = &a_info[i];
		a_ptr->cur_num = 0;
	}

	//EX建物パラメータ
	for(i=0;i<EX_BUILDINGS_PARAM_MAX;i++) ex_buildings_param[i] = 0;
	for(i=0;i<EX_BUILDINGS_HISTORY_MAX;i++) ex_buildings_history[i] = 0;

	/* Reset the objects */
	k_info_reset();

	/* Reset the "monsters" */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Hack -- Reset the counter */
		r_ptr->cur_num = 0;

		/* Hack -- Reset the max counter */
		r_ptr->max_num = 100;

		/* Hack -- Reset the max counter */
		if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->max_num = 1;

		/* Hack -- Non-unique Nazguls are semi-unique */
		else if (r_ptr->flags7 & RF7_NAZGUL) r_ptr->max_num = MAX_NAZGUL_NUM;

		/* Clear visible kills in this life */
		r_ptr->r_pkills = 0;

		/* Clear all kills in this life */
		r_ptr->r_akills = 0;

		//v1.1.30 ランダムユニークを初期化する。
		//初期化しなくても生成時に設定されるから問題ないだろうと思ってたが依姫のランダム神降ろしで前回ゲームのランダムユニークの技が出てきた
		if(IS_RANDOM_UNIQUE_IDX(i))
		{
			int j;
			my_strcpy(random_unique_name[i - MON_RANDOM_UNIQUE_1],"wiped_random_unique",sizeof(random_unique_name[i - MON_RANDOM_UNIQUE_1])-2);
			r_ptr->hdice = 1;
			r_ptr->hside = 1;
			r_ptr->ac = 10;
			r_ptr->sleep = 10;
			r_ptr->aaf = 10;
			r_ptr->speed = 110;
			r_ptr->mexp = 1;
			r_ptr->extra = 0;
			r_ptr->freq_spell = 0;
			r_ptr->flags1 = (RF1_FORCE_MAXHP);
			r_ptr->flags2 = 0L;
			r_ptr->flags3 = 0L;
			r_ptr->flags4 = 0L;
			r_ptr->flags5 = 0L;
			r_ptr->flags6 = 0L;
			r_ptr->flags7 = (RF7_UNIQUE2 | RF7_VARIABLE);
			if(i == MON_RANDOM_UNIQUE_3) r_ptr->flags7 |= (RF7_AQUATIC);
			r_ptr->flags8 = 0L;
			r_ptr->flags9 = 0L;
			r_ptr->flagsr = 0L;

			for(j=0;j<3;j++)
			{
				r_ptr->blow[j].method = RBM_HIT;
				r_ptr->blow[j].effect = RBE_HURT;
				r_ptr->blow[j].d_dice = 1;
				r_ptr->blow[j].d_side = 1;
			}
			r_ptr->level = 1;
			r_ptr->rarity = 1;
			r_ptr->r_flags1 = 0L;
			r_ptr->r_flags2 = 0L;
			r_ptr->r_flags3 = 0L;
			r_ptr->r_flags4 = 0L;
			r_ptr->r_flags5 = 0L;
			r_ptr->r_flags6 = 0L;
			r_ptr->r_flags9 = 0L;
			r_ptr->r_flagsr = 0L;
		}

	}

	/* Hack -- Well fed player */
	p_ptr->food = PY_FOOD_FULL - 1;


	/* Wipe the spells */
	///class スペマスが最初から全呪文習得済みになってる処理
	if (p_ptr->pclass == CLASS_SORCERER)
		///bug ↓この行有効にしたら新規プレイ時にフリーズ　この段階ではcp_ptrが有効でないせいらしい。となると特殊魔法習得職はどこで判定しよう。
	//if (REALM_SPELLMASTER)
	{
		p_ptr->spell_learned1 = p_ptr->spell_learned2 = 0xffffffffL;
		p_ptr->spell_worked1 = p_ptr->spell_worked2 = 0xffffffffL;
	}
	else
	{
		p_ptr->spell_learned1 = p_ptr->spell_learned2 = 0L;
		p_ptr->spell_worked1 = p_ptr->spell_worked2 = 0L;
	}
	p_ptr->spell_forgotten1 = p_ptr->spell_forgotten2 = 0L;
	for (i = 0; i < 64; i++) p_ptr->spell_order[i] = 99;
	p_ptr->learned_spells = 0;
	p_ptr->add_spells = 0;
	p_ptr->knowledge = 0;

	/* Clean the mutation count */
	mutant_regenerate_mod = 100;

	/* Clear "cheat" options */
	cheat_peek = FALSE;
	cheat_hear = FALSE;
	cheat_room = FALSE;
	cheat_xtra = FALSE;
	cheat_know = FALSE;
	cheat_live = FALSE;
	cheat_save = FALSE;

	/* Assume no winning game */
	p_ptr->total_winner = FALSE;

	world_player = FALSE;

	/* Assume no panic save */
	p_ptr->panic_save = 0;

	/* Assume no cheating */
	p_ptr->noscore = 0;
	p_ptr->wizard = FALSE;

	/* Not waiting to report score */
	p_ptr->wait_report_score = FALSE;

	/* Default pet command settings */
	p_ptr->pet_follow_distance = PET_FOLLOW_DIST;
	p_ptr->pet_extra_flags = (PF_TELEPORT | PF_ATTACK_SPELL | PF_SUMMON_SPELL);

	/* Wipe the recall depths */
	for (i = 0; i < max_d_idx; i++)
	{
		max_dlv[i] = 0;
		flag_dungeon_complete[i] = 0;
	}

	p_ptr->visit = 1;

	/* Reset wild_mode to FALSE */
	p_ptr->wild_mode = FALSE;

	//v1.1.94 magic_numサイズ変更
	for (i = 0; i < MAGIC_NUM_SIZE; i++)
	{
		p_ptr->magic_num1[i] = 0;
		p_ptr->magic_num2[i] = 0;
	}

	/* Level one */
	p_ptr->max_plv = p_ptr->lev = 1;

	///mod140423
	p_ptr->silent_floor = 0;
	p_ptr->maripo = 0;
	p_ptr->maripo_presented_flag = 0L;

	///mod140502
	p_ptr->lucky = 0;

	///mod140813
	p_ptr->asthma = 0;
	for (i = 0; i < 5; i++)p_ptr->race_multipur_val[i] = 0;

	/* Initialize arena and rewards information -KMW- */
	p_ptr->kourindou_number = 0;
	p_ptr->inside_arena = FALSE;
	p_ptr->inside_quest = 0;
	///class ものまね師の初期化
	for (i = 0; i < MAX_MANE; i++)
	{
		p_ptr->mane_spell[i] = -1;
		p_ptr->mane_dam[i] = 0;
	}
	p_ptr->mane_num = 0;
	p_ptr->exit_bldg = TRUE; /* only used for arena now -KMW- */

	/* Bounty */
	p_ptr->today_mon = 0;

	/* Reset monster arena */
	///sysdel tougi
	//battle_monsters();

	/* Reset mutations */
	p_ptr->muta1 = 0;
	p_ptr->muta2 = 0;
	p_ptr->muta3 = 0;
	p_ptr->muta1_perma = 0;
	p_ptr->muta2_perma = 0;
	p_ptr->muta3_perma = 0;
	//v1.1.64
	p_ptr->muta4 = 0;
	p_ptr->muta4_perma = 0;

	/* Reset virtues*/
	for (i = 0; i < 8; i++)
	{
			p_ptr->virtues[i]=0;
			p_ptr->vir_types[i] = 0;
	}

	/* Set the recall dungeon accordingly */
	if (vanilla_town)
	{
		dungeon_type = 0;
		p_ptr->recall_dungeon = DUNGEON_ANGBAND;
	}
	else
	{
		dungeon_type = 0;
		p_ptr->recall_dungeon = DUNGEON_TAISHI;
	}

	//v1.1.19
	p_ptr->monkey_count = 0;

	//v1.1.25
	score_bonus_kill_unique2 = 0L;

	//v1.1.52 実装時に追加し忘れてた
	qrkdr_rec_turn = 0;
	gambling_gain = 0;
	score_count_pinch = 0;
	score_count_pinch2 = 0;
	nightmare_diary_count = 0; 
	nightmare_diary_win = 0;	
	nightmare_total_point = 0;

	//v1.1.56
	p_ptr->tim_spellcard_count = 0;
	p_ptr->spellcard_effect_idx = 0;

	//v1.1.87

	ability_card_trade_count = 0;
	buy_gacha_box_count = 0;


}

/*
 *  Hook function for quest monsters
 */
/*:::ランクエ選定用hook*/
static bool mon_hook_quest(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Random quests are in the dungeon */
	if (r_ptr->flags8 & RF8_WILD_ONLY) return FALSE;

	/* No random quests for aquatic monsters */
	if (r_ptr->flags7 & RF7_AQUATIC) return FALSE;

	/* No random quests for multiplying monsters */
	if (r_ptr->flags2 & RF2_MULTIPLY) return FALSE;

	/* No quests to kill friendly monsters */
	if (r_ptr->flags7 & RF7_FRIENDLY) return FALSE;

	/*::: Hack モズグス様はランクエに出ない*/
	if(r_idx == MON_MOZGUS || r_idx == MON_MOZGUS2) return FALSE;

	//v1.1.42 紫苑はランクエに出ない
	if (r_idx == MON_SHION_1 || r_idx == MON_SHION_2) return FALSE;



	return TRUE;
}


/*
 * Determine the random quest uniques
 */
/*:::ランクエ用モンスターを選定する*/
///system ランクエ触るならこの辺
void determine_random_questor(quest_type *q_ptr)
{
	int          r_idx;
	monster_race *r_ptr;

	/* Prepare allocation table */
	get_mon_num_prep(mon_hook_quest, NULL);

	while (1)
	{
		/*
		 * Random monster 5 - 10 levels out of depth
		 * (depending on level)
		 */
		r_idx = get_mon_num(q_ptr->level + 5 + randint1(q_ptr->level / 10));
		r_ptr = &r_info[r_idx];

		if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

		///mod140301 生息地実装
		/*:::生息ダンジョンが指定されてるモンスターは出ないことにする*/
		if(r_ptr->extra != 0) continue;
		///mod140610 お尋ね者クエスト
		/*:::お尋ね者がランクエ対象にならないように設定する*/
		if(r_ptr->flags3 & RF3_WANTED) continue;
		if (r_ptr->flags1 & RF1_QUESTOR) continue;

		//2.0.13 ヤクザ戦争2クエストで賞金首設定されるかもしれない三人はランクエに選ばれない
		if (r_idx == MON_ENOKO) continue;
		if (r_idx == MON_BITEN) continue;
		if (r_idx == MON_CHIYARI) continue;

		//v1.1.59 針妙丸新性格　東方ユニークがランクエに選ばれない
		if (is_special_seikaku(SEIKAKU_SPECIAL_SHINMYOUMARU) && is_gen_unique(r_idx)) continue;

		///mod140721 低難易度では序盤クエストに強敵が出ない
		if(q_ptr->level < 45 && difficulty <= DIFFICULTY_NORMAL && (r_ptr->flags1 & RF1_FLAG_50F))	continue;

		if (r_ptr->rarity > 100) continue;

		if (r_ptr->flags7 & RF7_FRIENDLY) continue;
		if (r_ptr->flags3 & RF3_ANG_FRIENDLY) continue;
		if (r_ptr->flags3 & RF3_GEN_FRIENDLY) continue;

		if (r_ptr->flags7 & RF7_AQUATIC) continue;

		if (r_ptr->flags8 & RF8_WILD_ONLY) continue;

		if (no_questor_or_bounty_uniques(r_idx)) continue;



		/*
		 * Accept monsters that are 2 - 6 levels
		 * out of depth depending on the quest level
		 */
		if (r_ptr->level > (q_ptr->level + (q_ptr->level / 20))) break;
	}

	//test v2.0.13 詳細はcastle_questの未解決バグmemo
	//if (q_ptr->level == 38) r_idx = MON_BITEN;

	q_ptr->r_idx = r_idx;
}


/*
 *  Initialize random quests and final quests
 */
/*:::クエストの初期化　ランクエのモンスターを決定し、オベロンとサーペントのクエストを進行中状態にしている*/
static void init_dungeon_quests(void)
{
	//int number_of_quests = MAX_RANDOM_QUEST - MIN_RANDOM_QUEST + 1;
	///mod140305 50Fランクエを無くした
	int number_of_quests = 9;
	int i;

	/* Init the random quests */
	init_flags = INIT_ASSIGN;
	p_ptr->inside_quest = MIN_RANDOM_QUEST;

	/*:::この中でq_pref.txtも読み込まれる*/
	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	p_ptr->inside_quest = 0;

	/* Generate quests */
	//quest[40]-[48]がランクエに使われる
	for (i = MIN_RANDOM_QUEST + number_of_quests - 1; i >= MIN_RANDOM_QUEST; i--)
	{
		quest_type      *q_ptr = &quest[i];
		monster_race    *quest_r_ptr;

		q_ptr->status = QUEST_STATUS_TAKEN;

		if(EXTRA_MODE) continue; //Extraモードではランクエが別クエストになるので受領だけしてターゲットモンスターは決めない

		///quest ここ少し調整する必要アリ
		determine_random_questor(q_ptr);

		/* Mark uniques */
		quest_r_ptr = &r_info[q_ptr->r_idx];
		quest_r_ptr->flags1 |= RF1_QUESTOR;

		q_ptr->max_num = 1;
	}

	if(EXTRA_MODE)//Extraモードでは最初からオベロンクエスト受領し、紫クエストは未受領　ほかクエスト無効化処理もない
	{
		init_flags = INIT_ASSIGN;
		p_ptr->inside_quest = QUEST_TAISAI;
		process_dungeon_file("q_info.txt", 0, 0, 0, 0);
		quest[QUEST_TAISAI].status = QUEST_STATUS_TAKEN;
	}
	else
	{
		/*:::Mega Hack 一部ユニーククラスは特定クエストが最初から内部的に失敗になる*/
		for (i = 1; i < max_quests; i++)
		{
			if(check_ignoring_quest(i)) quest[i].status = QUEST_STATUS_FAILED_DONE;
		}
		/* Init the two main quests (Oberon + Serpent) */
		init_flags = INIT_ASSIGN;
		//p_ptr->inside_quest = QUEST_TAISAI;
		p_ptr->inside_quest = QUEST_YUKARI;

		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		//quest[QUEST_TAISAI].status = QUEST_STATUS_TAKEN;
		quest[QUEST_YUKARI].status = QUEST_STATUS_TAKEN;

		/*:::Hack - 藍は紫クエストを完了済み(スコアには参入しない)*/
		///mod160213 紫も同じにする
		if(p_ptr->pclass == CLASS_RAN || p_ptr->pclass == CLASS_YUKARI)
		{
			quest[QUEST_YUKARI].status = QUEST_STATUS_FINISHED;
			p_ptr->inside_quest = QUEST_TAISAI;
			init_flags |= INIT_ASSIGN;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			quest[QUEST_TAISAI].status = QUEST_STATUS_TAKEN;

		}
		/*:::Hack - 魔理沙専用クエスト*/
		else if(p_ptr->pclass == CLASS_MARISA)
		{
			p_ptr->inside_quest = QUEST_MOON_VAULT;
			init_flags |= INIT_ASSIGN;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			quest[QUEST_MOON_VAULT].status = QUEST_STATUS_TAKEN;

			p_ptr->inside_quest = QUEST_MARICON;
			init_flags |= INIT_ASSIGN;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			quest[QUEST_MARICON].status = QUEST_STATUS_TAKEN;
		}
	}

	//p_ptr->inside_quest = QUEST_SERPENT;
	//process_dungeon_file("q_info.txt", 0, 0, 0, 0);
	//quest[QUEST_SERPENT].status = QUEST_STATUS_TAKEN;
	p_ptr->inside_quest = 0;
}

/*
 * Reset turn
 */
/*:::ターンは一日目の朝6時から開始　アンデッドは0時からだが内部的には他よりターンが進んで開始らしい*/
static void init_turn(void)
{
#if 0
	if ((p_ptr->prace == RACE_VAMPIRE) ||
	    (p_ptr->prace == RACE_SKELETON) ||
	    (p_ptr->prace == RACE_ZOMBIE) ||
	    (p_ptr->prace == RACE_SPECTRE))
	{
		/* Undead start just after midnight */
		/*:::アンデッドは0時から開始　ターンを18時間進める　*/
		turn = (TURNS_PER_TICK*3 * TOWN_DAWN) / 4 + 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * MAX_DAYS + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}
	else
	{
		turn = 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}
#endif
	///mod1401 アンデッドが夜から開始する仕様を無くした
	turn = 1;
	turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;

	dungeon_turn = 1;
	dungeon_turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
}


/* 
 * Try to wield everything wieldable in the inventory. 
 * Code taken from Angband 3.1.0 under Angband license
 */ 
/*:::インベントリにあるアイテム全てを調べ、装備できるものを装備する　たいまつ除く　player_outfit()からのみ呼ばれる*/
static void wield_all(void) 
{ 
	object_type *o_ptr; 
	object_type *i_ptr; 
	object_type object_type_body; 
 
	int slot; 
	int item; 
 
	/* Scan through the slots backwards */ 
	for (item = INVEN_PACK - 1; item >= 0; item--) 
	{ 
		o_ptr = &inventory[item]; 
 
		/* Skip non-objects */ 
		if (!o_ptr->k_idx) continue; 
 
		/*:::装備品でないもの、光源、すでに何か装備している場所はパス*/
		/* Make sure we can wield it and that there's nothing else in that slot */ 
		if(p_ptr->pclass == CLASS_3_FAIRIES) slot = wield_slot_3_fairies(o_ptr); 
		else	slot = wield_slot(o_ptr); 

		if (slot < INVEN_RARM) continue; 
		if (o_ptr->tval == TV_LITE && o_ptr->sval == SV_LITE_TORCH && !object_is_artifact(o_ptr) ) continue; /* Does not wield toaches because buys a lantern soon */
		if (inventory[slot].k_idx) continue; 
 

		/* Get local object */ 
		i_ptr = &object_type_body; 
		object_copy(i_ptr, o_ptr); 
 
		/* Modify quantity */ 
		i_ptr->number = 1; 
 
		/* Decrease the item (from the pack) */ 
		if (item >= 0) 
		{ 
			inven_item_increase(item, -1); 
			inven_item_optimize(item); 
		} 
 
		/* Decrease the item (from the floor) */ 
		else 
		{ 
			floor_item_increase(0 - item, -1); 
			floor_item_optimize(0 - item); 
		} 
 
		/* Get the wield slot */ 
		o_ptr = &inventory[slot]; 
 
		/* Wear the new stuff */ 
		object_copy(o_ptr, i_ptr); 
 
		/* Increase the weight */ 
		/*:::なんかここで重量加算するのはおかしい気がする。まあどうせほどなく再計算されると思うが*/
		p_ptr->total_weight += i_ptr->weight; 
 
		/* Increment the equip counter by hand */ 
		equip_cnt++;

 	} 
	return; 
} 


/*
 * Each player starts out with a few items, given as tval/sval pairs.
 * In addition, he always has some food and a few torches.
 */
/*:::クラスごとの初期所持アイテム一覧*/
///class item クラスごとの初期所持アイテム一覧
#if 0
static byte player_init[MAX_CLASS][3][2] =
{
	{
		/* Warrior */
		{ TV_RING, SV_RING_RES_FEAR }, /* Warriors need it! */
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* Mage */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_DEATH_BOOK, 0 }, /* Hack: for realm2 book */
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Priest */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for Life / Death book */
		{ TV_DEATH_BOOK, 0 }, /* Hack: for realm2 book */
		{ TV_HAFTED, SV_MACE }
	},

	{
		/* Rogue */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Ranger */
		{ TV_NATURE_BOOK, 0 },
		{ TV_DEATH_BOOK, 0 },		/* Hack: for realm2 book */
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Paladin */
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SCROLL, SV_SCROLL_PROTECTION_FROM_EVIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* Warrior-Mage */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_DEATH_BOOK, 0 }, /* Hack: for realm2 book */
		{ TV_SWORD, SV_SHORT_SWORD }
	},

	{
		/* Chaos Warrior */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: For realm1 book */
		{ TV_HARD_ARMOR, SV_METAL_SCALE_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* Monk */
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POTION, SV_POTION_HEROISM }
	},

	{
		/* Mindcrafter */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_SMALL_SWORD }
	},

	{
		/* High Mage */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Tourist */
		{ TV_FOOD, SV_FOOD_JERKY},
		{ TV_SCROLL, SV_SCROLL_MAPPING },
		{ TV_BOW, SV_SLING}
	},

	{
		/* Imitator */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_SHORT_SWORD}
	},

	{
		/* Beastmaster */
		{ TV_TRUMP_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POLEARM, SV_SPEAR}
	},

	{
		/* Sorcerer */
		{ TV_HAFTED, SV_WIZSTAFF }, /* Hack: for realm1 book */
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_WAND, SV_WAND_MAGIC_MISSILE }
	},

	{
		/* Archer */
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Magic eater */
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Bard */
		{ TV_MUSIC_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Red Mage */
		{ TV_ARCANE_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Samurai */
		{ TV_HISSATSU_BOOK, 0 },
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* ForceTrainer */
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POTION, SV_POTION_RESTORE_MANA }
	},

	{
		/* Blue Mage */
		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Cavalry */
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL},
		{ TV_POLEARM, SV_BROAD_SPEAR}
	},

	{
		/* Berserker */
		{ TV_POTION, SV_POTION_HEALING },
		{ TV_HARD_ARMOR, SV_AUGMENTED_CHAIN_MAIL },
		{ TV_POLEARM, SV_BROAD_AXE }
	},

	{
		/* Weaponsmith */
		{ TV_RING, SV_RING_RES_FEAR }, /* Warriors need it! */
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_POLEARM, SV_BROAD_AXE }
	},
	{
		/* Mirror-Master */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_SWORD, SV_DAGGER }
	},
	{
		/* Ninja */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER }
	},
	{
		/* Sniper */
		{ TV_BOW, SV_LIGHT_XBOW },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER }
	},
};
#endif

/*
 * Hook function for human corpses
 */
static bool monster_hook_human(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags1 & (RF1_UNIQUE)) return FALSE;

	if (my_strchr("pht", r_ptr->d_char)) return TRUE;

	return FALSE;
}


/*
 * Add an outfit object
 */
/*:::player_outfitから呼ばれる。指定アイテムを鑑定しザックに追加し可能なら装備する*/
static void add_outfit(object_type *o_ptr)
{
	s16b slot;

	object_aware(o_ptr);
	object_known(o_ptr);
	///mod140821 初期所持品は全て*鑑定*されるように修正
	o_ptr->ident |= (IDENT_MENTAL);
	slot = inven_carry(o_ptr);

	/* Auto-inscription */
	autopick_alter_item(slot, FALSE);

	/* Now try wielding everything */ 
	wield_all(); 
}


/*:::player_outfit()から使う初期所持アイテム*/
/*:::種族ごと開始時所持品　食料や天狗の頭襟など*/
outfit_type birth_outfit_race[] = {
	{RACE_KARASU_TENGU,2,0,TV_HEAD,SV_HEAD_TOKIN,1},
	{RACE_HAKUROU_TENGU,2,0,TV_HEAD,SV_HEAD_TOKIN,1},

	{RACE_HUMAN,2,0,TV_FOOD,SV_FOOD_RATION,5},
	{RACE_YOUKAI,2,0,TV_FOOD,SV_FOOD_RATION,7},
	{RACE_HALF_YOUKAI,2,0,TV_FOOD,SV_FOOD_RATION,2},
	{RACE_SENNIN,2,0,TV_FOOD,SV_FOOD_SENTAN,3},
	{RACE_TENNIN,2,0,TV_SWEETS,SV_SWEETS_PEACH,6},
	{RACE_FAIRY,2,0,TV_SWEETS,SV_SWEET_COOKIE,4},
	{RACE_KAPPA,2,0,TV_FOOD,SV_FOOD_CUCUMBER,5},
	{RACE_KARASU_TENGU,2,0,TV_FOOD,SV_FOOD_HARDBUSC,6},
	{RACE_HAKUROU_TENGU,2,0,TV_FOOD,SV_FOOD_RATION,6},
	{RACE_ONI,2,0,TV_ALCOHOL,SV_ALCOHOL_ONIKOROSHI,5},
	{RACE_DEATH,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_WARBEAST,2,0,TV_FOOD,SV_FOOD_VENISON,20},
	{RACE_TSUKUMO,2,0,TV_STAFF,SV_STAFF_NOTHING,1},//小傘は持たない　特殊処理
	{RACE_ANDROID,2,0,TV_STAFF,SV_STAFF_NOTHING,1},//メディスンは持たなくする予定
	{RACE_GYOKUTO,2,0,TV_SWEETS,SV_SWEETS_DANGO,10},
	{RACE_YOUKO,2,0,TV_FOOD,SV_FOOD_ABURAAGE,3},
	{RACE_YAMAWARO,2,0,TV_FOOD,SV_FOOD_RATION,5},
	{RACE_BAKEDANUKI,2,0,TV_FOOD,SV_FOOD_RATION,6},
	{RACE_NINGYO,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_HOFGOBLIN,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_NYUDOU,2,0,TV_FOOD,SV_FOOD_RATION,7},
	{RACE_IMP,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_GOLEM,2,0,TV_MATERIAL,SV_MATERIAL_STONE,4},
	{RACE_KOBITO,2,0,TV_SWEETS,SV_SWEET_COOKIE,4},
	{RACE_ZOMBIE,2,0,TV_FOOD,SV_FOOD_SENTAN,3},
	{RACE_SPECTRE,2,0,TV_FOOD,SV_FOOD_RATION,2},
	{RACE_STRAYGOD,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_HALF_GHOST,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_DAIYOUKAI,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_GREATER_LILLIPUT,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_DEMIGOD,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{ RACE_DEITY,2,0,TV_SWEETS,SV_SWEETS_DANGO,10 },
	{RACE_LUNARIAN,2,0,TV_SWEETS,SV_SWEETS_PEACH,4},

	{-1,0,0,0,0,0} //終端dummy
};

/*:::職業ごと開始時所持品　装備品など*/
outfit_type birth_outfit_class[] = {
	{CLASS_WARRIOR,2,0,TV_SWORD,SV_WEAPON_LONG_SWORD,1},
	{CLASS_WARRIOR,2,0,TV_ARMOR,SV_ARMOR_SCALE,1},
	{CLASS_MAGE,2,0,TV_STICK,SV_WEAPON_QUARTERSTAFF,1},
	{CLASS_MAGE,2,0,TV_CLOTHES,SV_CLOTH_ROBE,1},
	{CLASS_HIGH_MAGE,2,0,TV_STICK,SV_WEAPON_QUARTERSTAFF,1},
	{CLASS_HIGH_MAGE,2,0,TV_CLOTHES,SV_CLOTH_ROBE,1},
	{CLASS_RANGER,2,0,TV_ARMOR,SV_ARMOR_LEATHER,1},
	{CLASS_RANGER,2,0,TV_KNIFE,SV_WEAPON_YAMAGATANA,1},
	{CLASS_RANGER,2,0,TV_BOW,SV_FIRE_SHORT_BOW,1},
	{CLASS_RANGER,2,0,TV_ARROW,SV_AMMO_LIGHT,20},
	{CLASS_MAID,0,0,TV_CLOTHES,SV_CLOTH_MAID,1},
	{CLASS_MAID,1,0,TV_CLOTHES,SV_CLOTH_SUIT,1},
	{CLASS_MAID,2,0,TV_KNIFE,SV_WEAPON_DAGGER,1},
	{CLASS_CIVILIAN,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},

	{CLASS_CAVALRY,2,0,TV_POLEARM,SV_WEAPON_GLAIVE,1},
	{CLASS_CAVALRY,2,0,TV_ARMOR,SV_ARMOR_SCALE,1},

	{ CLASS_RESEARCHER,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1 },

	{CLASS_ROGUE,2,0,TV_KNIFE,SV_WEAPON_TANTO,1},
	{CLASS_ROGUE,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1},
	{CLASS_ROGUE,2,0,TV_SCROLL,SV_SCROLL_MAPPING,5},

	{CLASS_ARCHER,2,0,TV_BOW,SV_FIRE_SHORT_BOW,1},
	{CLASS_ARCHER,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1},
	{CLASS_ARCHER,2,0,TV_ARROW,SV_AMMO_LIGHT,30},
	{CLASS_TEACHER,2,0,TV_SWORD,SV_WEAPON_RAPIER,1},
	{CLASS_TEACHER,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_PRIEST,2,0,TV_CLOTHES, SV_CLOTH_ROBE,1},
	{CLASS_PRIEST,2,0,TV_HAMMER,SV_WEAPON_MACE,1},
	{CLASS_OUTSIDER,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_OUTSIDER,2,0,TV_SOUVENIR,SV_SOUVENIR_SMARTPHONE,1},
	{CLASS_TSUKUMO_MASTER,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},
	{CLASS_TSUKUMO_MASTER,2,0,TV_AXE,SV_WEAPON_BROAD_AXE,1},
	{CLASS_SYUGEN,2,0,TV_CLOTHES, SV_CLOTH_SYUGEN,1},
	{CLASS_SYUGEN,2,0,TV_STICK, SV_WEAPON_SYAKUJYOU,1},
	{CLASS_MAGIC_KNIGHT,2,0,TV_ARMOR, SV_ARMOR_LEATHER,1},
	{CLASS_MAGIC_KNIGHT,2,0,TV_SWORD, SV_WEAPON_SABRE,1},
	{CLASS_LIBRARIAN,2,0,TV_SWORD,SV_WEAPON_RAPIER,1},
	{CLASS_LIBRARIAN,2,0,TV_CLOTHES,SV_CLOTH_SUIT,1},
	{CLASS_ENGINEER,2,0,TV_CLOTHES, SV_CLOTH_WORKER,1},
	{CLASS_ENGINEER,2,0,TV_HAMMER,SV_WEAPON_SPANNER,1},
	{CLASS_ENGINEER,2,0,TV_MACHINE,SV_MACHINE_WATERGUN,1},
	{CLASS_ENGINEER,2,0,TV_MACHINE,SV_MACHINE_LIGHTBOMB,1},
	{CLASS_SAMURAI,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},
	{CLASS_SAMURAI,2,0,TV_KATANA,SV_WEAPON_KATANA,1},
	{CLASS_MINDCRAFTER,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_PALADIN,2,0,TV_ARMOR, SV_ARMOR_CHAIN,1},
	{CLASS_PALADIN,2,0,TV_SWORD, SV_WEAPON_SHORT_SWORD,1},
	{CLASS_SH_DEALER,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_SH_DEALER,2,0,TV_HAMMER,SV_WEAPON_FLAIL,1},
	{CLASS_JEWELER,2,0,TV_KNIFE,SV_WEAPON_DAGGER,1},
	{CLASS_JEWELER,2,0,TV_CLOTHES,SV_CLOTH_ROBE,1},

	{CLASS_RUMIA,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_YOUMU,0,ART_ROUKANKEN,0,0,0},
	{CLASS_YOUMU,0,ART_HAKUROUKEN,0,0,0},
	{CLASS_YOUMU,2,0,TV_CLOTHES,SV_CLOTHES,1},

	{CLASS_UDONGE,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_UDONGE,2,0,TV_GUN,SV_FIRE_GUN_LUNATIC,1},
	{ CLASS_UDONGE,2,0,TV_HAMMER,SV_WEAPON_KINE,1 },
	{CLASS_UDONGE,2,0,TV_POTION,SV_POTION_CURE_SERIOUS,10},

	{CLASS_TEWI,2,0,TV_HAMMER,SV_WEAPON_KINE,1},
	{CLASS_TEWI,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_IKU,0,ART_HEAVENLY_MAIDEN,0,0,0},
	{CLASS_IKU,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_KOGASA,2,0,TV_CLOTHES,SV_CLOTHES,1},

	{CLASS_KOMACHI,2,0,TV_POLEARM,SV_WEAPON_WAR_SCYTHE,1},
	{CLASS_KOMACHI,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},

	{CLASS_KASEN,2,0,TV_CLOTHES,SV_CLOTHES,1},

	{CLASS_SUIKA,0,ART_IBUKI,0,0,0},
	{CLASS_SUIKA,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_MARISA,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_MARISA,2,0,TV_HEAD,SV_HEAD_WITCH,1},
	{CLASS_MARISA,2,0,TV_POTION,SV_POTION_SPEED,3},

	//魔理沙アビリティカード　専用性格以外ではキャンセルされる
	{ CLASS_MARISA,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_MARISA,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_MARISA,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },


	{CLASS_WAKASAGI,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},
	{CLASS_WAKASAGI,2,0,TV_MATERIAL,SV_MATERIAL_AQUAMARINE,1},

	{CLASS_KOISHI,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_KOISHI,2,0,TV_HEAD,SV_HEAD_HAT,1},
	{CLASS_SEIGA,2,0,TV_CLOTHES, SV_CLOTH_ROBE,1},

	{CLASS_MOMIZI,2,0,TV_CLOTHES, SV_CLOTH_SYUGEN,1},
	{CLASS_MOMIZI,2,0,TV_SWORD,SV_WEAPON_SCIMITAR,1},
	{CLASS_MOMIZI,2,0,TV_SHIELD,SV_SMALL_LEATHER_SHIELD,1},
	{CLASS_CIRNO,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_ORIN,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},
	{CLASS_ORIN,2,0,TV_MATERIAL, SV_MATERIAL_SKULL,1 },//v1.1.98

	{CLASS_SHINMYOUMARU,2,0,TV_CLOTHES, SV_CLOTH_EASTERN,1},
	{CLASS_SHINMYOUMARU,2,ART_SHINMYOUMARU,0,0,0},
	{CLASS_NAZRIN,0,ART_NAZRIN,0,0,0},
	{CLASS_NAZRIN,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_NAZRIN,2,0,TV_STICK, SV_WEAPON_IRONSTICK,1},

	{CLASS_LETTY,2,0,TV_CLOTHES, SV_CLOTHES,1}, 

	{CLASS_YOSHIKA,2,0,TV_CLOTHES, SV_CLOTHES,1}, 
	{CLASS_YOSHIKA,2,0,TV_FOOD, SV_FOOD_SENTAN,5}, 

	{CLASS_PATCHOULI,2,0,TV_CLOTHES, SV_CLOTH_ROBE,1}, 
	{CLASS_AYA,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_BANKI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_BANKI,2,0,TV_SPEAR, SV_WEAPON_AWL_PIKE,1},
	{CLASS_MYSTIA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_FLAN,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_FLAN,2,ART_FLAN,0, 0,1},

	{CLASS_SHOU,0,ART_HOUTOU,0,0,0},
	{CLASS_SHOU,2,0,TV_CLOTHES, SV_CLOTH_LEATHER	,1},
	{CLASS_SHOU,2,0,TV_SPEAR, SV_WEAPON_SPEAR,1},

	{CLASS_MEDICINE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_MEDICINE,2,0,TV_POTION, SV_POTION_POISON,16},

	{CLASS_YUYUKO,2,0,TV_CLOTHES, SV_CLOTH_EASTERN,1},
	{CLASS_YUYUKO,2,0,TV_FOOD, SV_FOOD_RATION,20},

	{CLASS_SATORI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KYOUKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_TOZIKO,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_KISUME,2,0,TV_ARMOR, SV_ARMOR_TUB,1},
	{CLASS_LILY_WHITE,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_HATATE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_HATATE,2,0,TV_CROSSBOW,SV_FIRE_LIGHT_CROSSBOW,1},
	{CLASS_HATATE,2,0,TV_BOLT,SV_AMMO_LIGHT,20},

	{CLASS_MIKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KOKORO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_WRIGGLE,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_NINJA,2,0,TV_KNIFE,SV_WEAPON_TANTO,1},
	{CLASS_NINJA,2,0,TV_CLOTHES, SV_CLOTH_KUROSHOUZOKU,1},
	{CLASS_NINJA,2,0,TV_SCROLL,SV_SCROLL_DARKNESS,7},
	{CLASS_CHEN,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_YUUKA,0,ART_YUUKA,0,0,0},
	{CLASS_YUUKA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_MURASA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KEINE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KEINE,2,0,TV_SWORD,SV_WEAPON_LONG_SWORD,1},

	{CLASS_REIMU,2,0,TV_STICK,SV_WEAPON_OONUSA,1},
	{CLASS_REIMU,2,0,TV_CLOTHES,SV_CLOTH_MIKO,1},

	{CLASS_YUGI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_YUGI,0,ART_HOSHIGUMA,0,0,0},

	{CLASS_KAGEROU,2,0,TV_CLOTHES, SV_CLOTH_ROBE,1},
	{CLASS_SHIZUHA,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},

	{CLASS_SANAE,2,0,TV_STICK,SV_WEAPON_QUARTERSTAFF,1},
	{CLASS_SANAE,2,0,TV_CLOTHES,SV_CLOTH_MIKO,1},

	{CLASS_MINORIKO,2,0,TV_STICK,SV_WEAPON_QUARTERSTAFF,1},
	{CLASS_MINORIKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_REMY,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},

	{CLASS_NITORI,2,0,TV_CLOTHES, SV_CLOTH_WORKER,1},
	{CLASS_NITORI,2,0,TV_HAMMER,SV_WEAPON_SPANNER,1},
	{CLASS_NITORI,2,0,TV_MACHINE,SV_MACHINE_WATERGUN,1},

	{CLASS_BYAKUREN,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},
	{CLASS_SUWAKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_NUE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_NUE,2,0,TV_SPEAR, SV_WEAPON_TRIFURCATE_SPEAR,1},

	{CLASS_SEIJA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_SEIJA,2,0,TV_WAND, SV_WAND_MAGIC_MISSILE,1},
	{CLASS_SEIJA,2,0,TV_WAND, SV_WAND_STINKING_CLOUD,1},
	{CLASS_SEIJA,2,0,TV_WAND, SV_WAND_SLEEP_MONSTER,1},

	{CLASS_UTSUHO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_YAMAME,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_MARTIAL_ARTIST,2,0,TV_CLOTHES, SV_CLOTH_DOUGI,1},

	{CLASS_MAGIC_EATER,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_MAGIC_EATER,2,0,TV_WAND, SV_WAND_MAGIC_MISSILE,1},
	{CLASS_MAGIC_EATER,2,0,TV_WAND, SV_WAND_STINKING_CLOUD,1},

	{CLASS_RAN,2,0,TV_CLOTHES, SV_CLOTH_DOUSHI,1},
	{CLASS_RAN,2,0,TV_KNIFE,SV_WEAPON_TANTO,1},
	{CLASS_RAN,2,0,TV_SCROLL, SV_SCROLL_WORD_OF_RECALL,9},

	{CLASS_EIKI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_EIKI,0,ART_EIKI,0,0,0},

	{CLASS_TENSHI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_TENSHI,0,ART_HISOU,0,0,0},

	{CLASS_MEIRIN,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_PARSEE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_PARSEE,2,0,TV_KATANA, SV_WEAPON_SHORT_KATANA,1},

	{CLASS_SHINMYOU_2,2,0,TV_CLOTHES, SV_CLOTH_EASTERN,1},
	{CLASS_SHINMYOU_2,2,ART_SHINMYOUMARU,0,0,0},

	{CLASS_SUMIREKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_SUMIREKO,2,ART_3DPRINTER_GUN,0, 0,0},

	{CLASS_ICHIRIN,2,0,TV_CLOTHES,SV_CLOTH_ROBE,1},
	{CLASS_MOKOU,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_KANAKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KANAKO,2,0,TV_STICK,SV_WEAPON_SIXFEET,1},

	{CLASS_FUTO,2,0,TV_CLOTHES, SV_CLOTH_DOUSHI,1},
	{CLASS_FUTO,2,0,TV_BOW,SV_FIRE_SHORT_BOW,1},
	{CLASS_FUTO,2,0,TV_ARROW,SV_AMMO_LIGHT,20},

	{CLASS_SUNNY,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_LUNAR,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_STAR,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_ALICE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_LUNASA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_MERLIN,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_LYRICA,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_CLOWNPIECE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_CLOWNPIECE,2,ART_CLOWNPIECE,0,0,0},

	{CLASS_DOREMY,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_SAKUYA,0,0,TV_CLOTHES,SV_CLOTH_MAID,1},
	{CLASS_BENBEN,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_YATSUHASHI,2,0,TV_CLOTHES,SV_CLOTHES,1},
	//{CLASS_HINA,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},特殊処理で最初から呪いにする
	{CLASS_3_FAIRIES,2,0,TV_CLOTHES, SV_CLOTHES,1},//3回装備処理を行うために3つ書く
	{CLASS_3_FAIRIES,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_3_FAIRIES,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{ CLASS_3_FAIRIES,2,ART_CLOWNPIECE,0,0,0 },//専用性格以外では除外処理する
	{ CLASS_3_FAIRIES,2,0,TV_FOOD, SV_FOOD_MAGIC_WATERMELON,1 },
	{ CLASS_3_FAIRIES,2,0,TV_MUSHROOM, SV_MUSHROOM_PUFFBALL,1 },


	{CLASS_RAIKO,2,0,TV_CLOTHES,SV_CLOTH_SUIT,1},

	{CLASS_CHEMIST,2,0,TV_KNIFE,SV_WEAPON_YAMAGATANA,1},
	{CLASS_CHEMIST,2,0,TV_CLOTHES,SV_CLOTH_SUIT,1},
	{CLASS_CHEMIST,2,0,TV_BOOK_MEDICINE,0,1},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_RED,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_WHITE,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_GREEN,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_BLUE,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_BLACK,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_POISON,5},
	{CLASS_MAMIZOU,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_YUKARI,0,ART_YUKARI,0,0,0},
	{CLASS_YUKARI,2,0,TV_CLOTHES,SV_CLOTH_DRESS,1},

	{CLASS_RINGO,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_RINGO,2,0,TV_GUN,SV_FIRE_GUN_LUNATIC,1},
	{CLASS_RINGO,2,0,TV_SWEETS,SV_SWEETS_DANGO,30},

	{CLASS_SEIRAN,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_SEIRAN,2,0,TV_GUN,SV_FIRE_GUN_LUNATIC,1},
	{CLASS_SEIRAN,2,0,TV_HAMMER,SV_WEAPON_KINE,1},

	{CLASS_EIRIN,2,0,TV_BOW,SV_FIRE_SHORT_BOW,1},
	{CLASS_EIRIN,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_EIRIN,2,0,TV_ARROW,SV_AMMO_LIGHT,30},

	{CLASS_KAGUYA,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},
	//輝夜難題　特殊処理でON/OFFする
	{CLASS_KAGUYA,0,ART_KAGUYA_DRAGON,0,0,0},
	{CLASS_KAGUYA,0,ART_KAGUYA_HOURAI,0,0,0},
	{CLASS_KAGUYA,0,ART_KAGUYA_HOTOKE,0,0,0},
	{CLASS_KAGUYA,0,ART_KAGUYA_HINEZUMI,0,0,0},
	{CLASS_KAGUYA,0,ART_KAGUYA_TSUBAME,0,0,0},

	{CLASS_SAGUME,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},

	{CLASS_REISEN2,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_REISEN2,2,0,TV_GUN,SV_FIRE_GUN_INFANTRY,1},

	{CLASS_TOYOHIME,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_TOYOHIME,2,ART_TOYOHIME,0,0,0},
	{CLASS_TOYOHIME,2,0,TV_SWEETS,SV_SWEETS_PEACH,8},

	{CLASS_YORIHIME,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_YORIHIME,2,ART_YORIHIME,0,0,0},

	{CLASS_JUNKO,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_HECATIA,2,ART_HECATIA,0,0,0},

	{CLASS_SOLDIER,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1},
	{CLASS_SOLDIER,2,0,TV_KNIFE,SV_WEAPON_KNIFE,1},
	{CLASS_SOLDIER,2,0,TV_GUN,SV_FIRE_GUN_HANDGUN,1},

	{CLASS_NEMUNO,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_NEMUNO,2,0,TV_AXE,SV_WEAPON_NATA,1},

	{CLASS_AUNN,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_NARUMI,2,0,TV_CLOTHES,SV_CLOTHES,1},

	//v1.1.37 小鈴は色々巻物持ってる
	{CLASS_KOSUZU,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_RUNE_OF_PROTECTION,5},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_DISPEL_UNDEAD,5},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_STAR_DESTRUCTION,5},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_FIRE,3},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_ICE,3},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_CHAOS,3},
	{CLASS_KOSUZU,2,0,TV_BOOK_OCCULT,3,1},
	{CLASS_KOSUZU,2,0,TV_SOUVENIR,SV_SOUVENIR_OUIJA_BOARD,1},
	{CLASS_KOSUZU,2,ART_HYAKKI,0,0,1},

	{ CLASS_MAI,2,0,TV_CLOTHES,SV_CLOTH_DRESS,1 },
	{ CLASS_MAI,2,0,TV_SPEAR,SV_WEAPON_SPEAR,1 },

	{ CLASS_SATONO,2,0,TV_CLOTHES,SV_CLOTH_DRESS,1 },

	{ CLASS_VFS_CLOWNPIECE,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_VFS_CLOWNPIECE,2,ART_CLOWNPIECE,0,0,0 },

	{ CLASS_JYOON,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1 },
	{ CLASS_SHION,2,0,TV_CLOTHES, SV_CLOTHES,1 },

	{ CLASS_OKINA,2,0,TV_CLOTHES, SV_CLOTH_DOUSHI,1 },
	{ CLASS_OKINA,2,ART_OKINA,0, 0,1 },
	{ CLASS_EIKA,2,0,TV_CLOTHES,SV_CLOTHES,1 },

	{ CLASS_MAYUMI,2,0,TV_ARMOR,SV_ARMOR_FINE_CERAMIC,1 },
	{ CLASS_MAYUMI,2,ART_MAYUMI,0,0,1 },

	{ CLASS_KUTAKA,2,0,TV_CLOTHES,SV_CLOTHES,1 },

	{ CLASS_URUMI,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_URUMI,2,0,TV_OTHERWEAPON,SV_OTHERWEAPON_FISHING,1 },
	{ CLASS_URUMI,2,0,TV_SOUVENIR,SV_SOUVENIR_STONE_BABY,1 },

	{ CLASS_SAKI,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_SAKI,2,0,TV_GUN,SV_FIRE_GUN_HANDGUN,1 },

	{ CLASS_YACHIE,2,0,TV_CLOTHES,SV_CLOTHES,1 },

	{ CLASS_KEIKI,2,ART_KEIKI,0,0,1 },
	{ CLASS_KEIKI,2,0,TV_KNIFE,SV_WEAPON_KNIFE,2 },

	{ CLASS_TAKANE,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1 },
	{ CLASS_TAKANE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_TAKANE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_TAKANE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },

	{ CLASS_MIKE,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_MIKE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_MIKE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_MIKE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },


	{ CLASS_CARD_DEALER,2,0,TV_CLOTHES,SV_CLOTH_SUIT,1 },
	{ CLASS_CARD_DEALER,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_CARD_DEALER,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_CARD_DEALER,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },

	{ CLASS_MOMOYO,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_MOMOYO,2,0,TV_AXE,SV_WEAPON_PICK,1 },
	{ CLASS_MOMOYO,2,0,TV_MATERIAL,SV_MATERIAL_STONE,3 },

	{ CLASS_SANNYO,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1 },

	{ CLASS_TSUKASA,2,0,TV_CLOTHES,SV_CLOTHES,1 },

	{ CLASS_MEGUMU,2,0,TV_CLOTHES, SV_CLOTH_SYUGEN,1 },
	{ CLASS_MEGUMU,2,ART_MEGUMU,0,0,1 },

	{ CLASS_MISUMARU,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_MISUMARU,2,0,TV_MATERIAL,SV_MATERIAL_HEMATITE,1 },
	{ CLASS_MISUMARU,2,0,TV_MATERIAL,SV_MATERIAL_MAGNETITE,1 },
	{ CLASS_MISUMARU,2,0,TV_MATERIAL,SV_MATERIAL_OPAL,1 },

	{ CLASS_YUMA,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_YUMA,2,ART_TOUTETSU,0,0,1 },
	{ CLASS_YUMA,2,0,TV_FLASK, SV_FLASK_OIL,20 },

	{ CLASS_CHIMATA,2,0,TV_CLOTHES, SV_CLOTHES,1 },

	{ CLASS_MIYOI,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_MIYOI,2,ART_IBUKI,0,0,1 },

	{ CLASS_BITEN,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_BITEN,2,ART_BITEN,0,0,1 },
	{ CLASS_BITEN,2,0,TV_ALCOHOL, SV_ALCOHOL_NERIZAKE,1 },

	{ CLASS_ENOKO,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_ENOKO,2,ART_ENOKO,0,0,1 },

	{ CLASS_CHIYARI,2,0,TV_CLOTHES, SV_CLOTH_T_SHIRT,1 },

	{ CLASS_HISAMI,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_ZANMU,2,0,TV_CLOTHES, SV_CLOTHES,1 },

	{ CLASS_BEEKEEPER,2,0,TV_CLOTHES, SV_CLOTH_PADDED,1 },
	{ CLASS_BEEKEEPER,2,0,TV_AXE, SV_WEAPON_NATA,1 },
	{ CLASS_BEEKEEPER,2,0,TV_SWEETS, SV_SWEETS_HONEY,10 },
	{ CLASS_BEEKEEPER,2,0,TV_POTION, SV_POTION_CURE_POISON,3 },

	{ CLASS_DAIYOUSEI,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	//{ CLASS_DAIYOUSEI,2,0,TV_STICK, SV_WEAPON_FLOWER,1 }, 個別処理で上質生成する

	{ CLASS_MIZUCHI,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_MIZUCHI,2,ART_HARNESS_HELL,0, 0,1 },

	{ CLASS_UBAME,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_CHIMI,2,0,TV_CLOTHES, SV_CLOTHES,1 },

	{ CLASS_NAREKO,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_NAREKO,2,0,TV_HAMMER, SV_WEAPON_MACE,1 },

	{-1,0,0,0,0,0} //終端dummy
};



/*
 * Init players with some belongings
 *
 * Having an item makes the player "aware" of its purpose.
 */
/*:::職業、種族、性格による初期所持アイテムを決定する*/
///race class realm item 初期アイテム
void player_outfit(void)
{
	int i, tv, sv;

	object_type	forge;
	object_type	*q_ptr;

	/* Get local object */
	q_ptr = &forge;

	/*:::小傘特殊処理　専用可変パラメータアーティファクト*/
	if(p_ptr->pclass == CLASS_KOGASA)
	{
		object_prep(q_ptr, lookup_kind(TV_STICK, SV_WEAPON_KOGASA));
		apply_kogasa_magic(q_ptr,1,TRUE);
		add_outfit(q_ptr);
	}
	/*:::アリス特殊処理　上海人形にレイピア装備*/
	else if(p_ptr->pclass == CLASS_ALICE)
	{
		object_prep(q_ptr, lookup_kind(TV_SWORD, SV_WEAPON_RAPIER));
		apply_magic(q_ptr,1,AM_FORCE_NORMAL);
		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		object_copy(&inven_add[0], q_ptr); 

	}
	/*:::咲夜特殊処理*/
	else if(p_ptr->pclass == CLASS_SAKUYA)
	{
		object_prep(q_ptr, lookup_kind(TV_KNIFE, SV_WEAPON_KNIFE));
		q_ptr->name2 = EGO_WEAPON_DEMONSLAY;
		q_ptr->pval = 1;
		q_ptr->to_d = 10;
		q_ptr->to_h = 10;

		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		add_outfit(q_ptr);

	}
	//v1.1.68 三月精専用性格は、服を通常の3着に追加しさらに6着
	else if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
	{
		for (i = 0; i < 6; i++)
		{
			object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTHES));
			apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
			object_aware(q_ptr);
			object_known(q_ptr);
			q_ptr->ident |= (IDENT_MENTAL);
			add_outfit(q_ptr);

		}


	}
	/*:::雛特殊処理*/
	else if(p_ptr->pclass == CLASS_HINA)
	{
		object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_DRESS));
		apply_magic(q_ptr,1,AM_FORCE_NORMAL);
		q_ptr->curse_flags = TRC_CURSED;
		q_ptr->to_a = 3;
		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		add_outfit(q_ptr);

	}
	//v1.1.88 たかね初期所持カードに自前のを一枚追加
	else if (p_ptr->pclass == CLASS_TAKANE)
	{
		object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		apply_magic_abilitycard(q_ptr, ABL_CARD_YAMAWARO, 0, 0);
		add_outfit(q_ptr);

	}
	//v1.1.90 山如も
	else if (p_ptr->pclass == CLASS_SANNYO)
	{
		object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		apply_magic_abilitycard(q_ptr, ABL_CARD_KISERU, 0, 0);
		add_outfit(q_ptr);

	}
	//v1.1.93 ミケも
	else if (p_ptr->pclass == CLASS_MIKE)
	{
		object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		apply_magic_abilitycard(q_ptr, ABL_CARD_MANEKINEKO, 0, 0);
		add_outfit(q_ptr);

	}
	//v2.0.20 大妖精の花
	else if (p_ptr->pclass == CLASS_DAIYOUSEI)
	{
		object_prep(q_ptr, lookup_kind(TV_STICK, SV_WEAPON_FLOWER));
		q_ptr->pval = 2;
		q_ptr->to_d = 10;
		q_ptr->to_h = 10;

		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		add_outfit(q_ptr);

	}


	//v1.1.87 カード売人系の職業がEXTRAモードで開始したとき10連ボックスを追加で持っている
	//v1.1.87c CLASS_CARD_DEALERとCHECK_ABLCARD_DEALER_CLASSを間違えて全キャラで箱が手に入っていた。修正。
	if (EXTRA_MODE && CHECK_ABLCARD_DEALER_CLASS)
	{
		object_prep(q_ptr, lookup_kind(TV_CHEST, SV_CHEST_10_GACHA));
		apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		add_outfit(q_ptr);

	}






	//v1.1.79 新種族「座敷わらし」の初期の移動屋敷
	//v2.0.9 美宵は伊吹瓢固定。★初期所持処理から装備する
	if (p_ptr->prace == RACE_ZASHIKIWARASHI && p_ptr->pclass != CLASS_MIYOI)
	{
		object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_INN));
		object_copy(&inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE], q_ptr);

	}

	//種族固定の初期アイテムを生成
	for(i=0;birth_outfit_race[i].type != -1 ;i++)
	{
		//種族と性別が合わない行はパス
		if(birth_outfit_race[i].type != p_ptr->prace) continue;
		if(birth_outfit_race[i].psex == SEX_MALE && p_ptr->psex == SEX_FEMALE || birth_outfit_race[i].psex == SEX_FEMALE && p_ptr->psex == SEX_MALE) continue;
		//-Hack-小傘は付喪神だが杖を食べられないので持っていない
		if(birth_outfit_race[i].tval==TV_STAFF && birth_outfit_race[i].sval==SV_STAFF_NOTHING && p_ptr->pclass == CLASS_KOGASA) continue;
		//-Hack-メディスンも杖を食べられないので持っていない
		if(p_ptr->pclass == CLASS_MEDICINE) break;
		//外来人は食糧を持たない
		if(p_ptr->pclass == CLASS_OUTSIDER) break;
		//ドレミーも食糧を持たない
		if(p_ptr->pclass == CLASS_DOREMY) break;
		//v1.1.80 文専用性格は頭襟を持たない
		if (is_special_seikaku(SEIKAKU_SPECIAL_AYA)) break;

		//種族処理にアーティファクトは未対応
		if(birth_outfit_race[i].artifact_idx)
		{
			msg_format("ERROR:birth_outfit_race[%d]にアーティファクトが指定されている(未実装)",i);
			continue;
		}
		//念のため
		if(birth_outfit_race[i].num < 1 || birth_outfit_race[i].num > 99 )
		{
			msg_format("ERROR:birth_outfit_race[%d]のアイテム数がおかしい",i);
			continue;
		}
		object_prep(q_ptr,lookup_kind(birth_outfit_race[i].tval,birth_outfit_race[i].sval));
			//apply_magicの★判定と上質判定をキャンセルし並みの品を生成。装備品はapply_magicしなければ勝手に並になるが光源とかの処理のため
		apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
		q_ptr->number = birth_outfit_race[i].num;
		add_outfit(q_ptr);
	}

	//職業（ユニーククラス含む）固定の初期アイテム
	for(i=0;birth_outfit_class[i].type != -1 ;i++)
	{
		//職業と性別が合わない行はパス
		if(birth_outfit_class[i].type != p_ptr->pclass) continue;
		if(birth_outfit_class[i].psex == SEX_MALE && p_ptr->psex == SEX_FEMALE || birth_outfit_class[i].psex == SEX_FEMALE && p_ptr->psex == SEX_MALE) continue;


		//v2.0.1 魔理沙は専用性格以外ではアビリティカードを持たない
		if (p_ptr->pclass == CLASS_MARISA && !is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
		{
			if (birth_outfit_class[i].tval == TV_ABILITY_CARD) continue;
		}

		//アーティファクトを持って開始するクラスの処理 大部分はcreate_named_art()からコピー
		if(birth_outfit_class[i].artifact_idx)
		{
			int tmp_k_idx;
			artifact_type *a_ptr = &a_info[birth_outfit_class[i].artifact_idx];

			//★が出ないオプションがONのとき、
			//player_outfit()の直前に全ての★のcur_numが1になって生成されなくなる
			//v2.0.9 例外として美宵の家のみ生成可能にする
			if(a_ptr->cur_num && p_ptr->pclass != CLASS_MIYOI) continue;

			if (!a_ptr->name)
			{
				msg_format("ERROR:birth_outfit_class[%d]のアーティファクトIDがおかしい",i);
				continue;
			}
			tmp_k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);
			if (!tmp_k_idx)
			{
				msg_format("ERROR:a_info[%d]のtvalかsvalがおかしい",i);
				continue;
			}

			object_prep(q_ptr, tmp_k_idx);
			q_ptr->name1 = birth_outfit_class[i].artifact_idx;

			// Hack - 輝夜特殊処理　まだ輝夜クエストで届けられていないアイテムを除外
			if(p_ptr->pclass == CLASS_KAGUYA)
			{
				if(q_ptr->name1 == ART_KAGUYA_TSUBAME && kaguya_quest_level < 1) continue;
				if(q_ptr->name1 == ART_KAGUYA_HINEZUMI && kaguya_quest_level < 2) continue;
				if(q_ptr->name1 == ART_KAGUYA_HOTOKE && kaguya_quest_level < 3) continue;
				if(q_ptr->name1 == ART_KAGUYA_DRAGON && kaguya_quest_level < 4) continue;
				if(q_ptr->name1 == ART_KAGUYA_HOURAI && kaguya_quest_level < 5) continue;
			}

			//三月精は専用性格以外ではクラウンピースの松明を持たない
			if (p_ptr->pclass == CLASS_3_FAIRIES)
			{
				if (!is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES)) continue;
			}

			/* Extract the fields */
			q_ptr->pval = a_ptr->pval;
			q_ptr->ac = a_ptr->ac;
			q_ptr->dd = a_ptr->dd;
			q_ptr->ds = a_ptr->ds;
			q_ptr->to_a = a_ptr->to_a;
			q_ptr->to_h = a_ptr->to_h;
			q_ptr->to_d = a_ptr->to_d;
			q_ptr->weight = a_ptr->weight;

			///mod150908 クラウンピースの松明の特殊処理
			if(q_ptr->name1 == ART_CLOWNPIECE) q_ptr->xtra4 = -1;

			/* Hack -- extract the "cursed" flag */
			if (a_ptr->gen_flags & TRG_CURSED) q_ptr->curse_flags |= (TRC_CURSED);
			if (a_ptr->gen_flags & TRG_HEAVY_CURSE) q_ptr->curse_flags |= (TRC_HEAVY_CURSE);
			if (a_ptr->gen_flags & TRG_PERMA_CURSE) q_ptr->curse_flags |= (TRC_PERMA_CURSE);
			if (a_ptr->gen_flags & (TRG_RANDOM_CURSE0)) q_ptr->curse_flags |= get_curse(0, q_ptr);
			if (a_ptr->gen_flags & (TRG_RANDOM_CURSE1)) q_ptr->curse_flags |= get_curse(1, q_ptr);
			if (a_ptr->gen_flags & (TRG_RANDOM_CURSE2)) q_ptr->curse_flags |= get_curse(2, q_ptr);
			random_artifact_resistance(q_ptr, a_ptr);

			//この二行要るのか要らないのか厳密には見てないがとりあえず追加
			q_ptr->number = 1;
			a_info[birth_outfit_class[i].artifact_idx].cur_num+=1;

			q_ptr->ident |= (IDENT_MENTAL);

			//v2.0.9 美宵の「伊吹瓢」特殊処理
			if (p_ptr->pclass == CLASS_MIYOI)
			{
				object_aware(q_ptr);
				object_known(q_ptr);
				q_ptr->ident |= (IDENT_MENTAL);
				object_copy(&inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE], q_ptr);

			}
			else
			{
				add_outfit(q_ptr);
			}

		}	
		//念のため
		else if(birth_outfit_class[i].num < 1 || birth_outfit_class[i].num > 99 )
		{
			msg_format("ERROR:birth_outfit_class[%d]のアイテム数がおかしい",i);
			continue;
		}		
		else
		{

			int tmp_num;
			object_prep(q_ptr,lookup_kind(birth_outfit_class[i].tval,birth_outfit_class[i].sval));
			//apply_magicの★判定と上質判定をキャンセルし並みの品を生成。装備品はapply_magicしなければ勝手に並になるが光源とかの処理のため
			apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
			//個数処理
			//v1.1.62 EXTRAでは矢玉類の初期所持数を増やす
			tmp_num = birth_outfit_class[i].num;
			if (EXTRA_MODE && (q_ptr->tval == TV_ARROW || q_ptr->tval == TV_BOLT))
				tmp_num = MIN(99, tmp_num * 3);
			q_ptr->number = tmp_num;
			//pvalのある武器防具は常に1
			if(object_is_weapon_armour_ammo(q_ptr) && q_ptr->pval) q_ptr->pval = 1;
			add_outfit(q_ptr);
		}
	}
	//読める魔法書の一冊目を持つ
	if(p_ptr->realm1)
	{
		object_prep(q_ptr,lookup_kind(p_ptr->realm1,0));
		q_ptr->number = 1;
		if(EXTRA_MODE) q_ptr->number += randint1(2);
		add_outfit(q_ptr);
	}
	if(p_ptr->realm2)
	{
		object_prep(q_ptr,lookup_kind(p_ptr->realm2,0));
		q_ptr->number = 1;
		if(EXTRA_MODE) q_ptr->number += randint1(2);
		add_outfit(q_ptr);
	}
	//v2.1.1 もし異変領域を選択した場合は初期所持以外のsval1～7の石をランダムに追加取得
	//ハイメイジは3つ追加取得
	if (p_ptr->realm1 == TV_STONE_INCIDENT || p_ptr->realm2 == TV_STONE_INCIDENT)
	{
		int tmp_sv[3];
		int num = (p_ptr->pclass == CLASS_HIGH_MAGE) ? 3 : 1;

		for (i = 0; i < num; i++)
		{
			//石の種類が重複しないよう雑に処理
			while (TRUE)
			{
				tmp_sv[i] = randint1(SV_INCIDENT_STONE_MAX);
				if (i == 0) break;
				if (i == 1 && tmp_sv[1] != tmp_sv[0]) break;
				if (i == 2 && tmp_sv[2] != tmp_sv[0] && tmp_sv[2] != tmp_sv[1]) break;
			}

			object_prep(q_ptr, lookup_kind(TV_STONE_INCIDENT, tmp_sv[i]));
			q_ptr->number = 1;
			if (EXTRA_MODE) q_ptr->number += randint1(2);
			add_outfit(q_ptr);

		}

	}

	//v1.1.44 うどんげ特殊性格
	if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
	{
		object_prep(q_ptr, lookup_kind(TV_BOOK_MEDICINE, 0));
		q_ptr->number = 1;
		if (EXTRA_MODE) q_ptr->number += randint1(2);
		add_outfit(q_ptr);
	}
	//v1.1.52 菫子特殊性格
	else if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
	{
		object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SMARTPHONE));
		q_ptr->number = 1;
		add_outfit(q_ptr);
	}


	if(REALM_SPELLMASTER)
	{
		for(i=1;i<=TV_BOOK_END;i++)
		{
			bool flag_ok = FALSE;
			//v1.1.32 パチュリー特殊性格「書痴」追加
			if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI) && i <= MAX_BASIC_MAGIC_REALM)
				flag_ok = TRUE;
			if (cp_ptr->realm_aptitude[i] && rp_ptr->realm_aptitude[i])
				flag_ok = TRUE;

			if(flag_ok)
			{
				object_prep(q_ptr,lookup_kind(i,0));
				q_ptr->number = 1;
				if(EXTRA_MODE) q_ptr->number += randint1(2);
				add_outfit(q_ptr);
			}
		}
	}


	//たいまつ
	if (p_ptr->pclass != CLASS_NINJA && p_ptr->pclass != CLASS_OUTSIDER  && p_ptr->pclass != CLASS_NUE
		&& p_ptr->prace != RACE_VAMPIRE && p_ptr->pclass != CLASS_WRIGGLE && p_ptr->pclass != CLASS_UTSUHO)
	{
		object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH));


		//v1.1.93 たいまつの性能と数を揃える。EXTRAで引きが悪くてQy@するのもつまらんので
		//q_ptr->number = (byte)rand_range(3, 7);
		//if(EXTRA_MODE) q_ptr->number += 3;
		//q_ptr->xtra4 = rand_range(3, 7) * 500;
		q_ptr->number = (EXTRA_MODE) ? 9 : 6;
		q_ptr->xtra4 = 2500;
		add_outfit(q_ptr);
	}

	//Exモードではツルハシを持っている。たまに扉のない部屋に出るため。
	if(EXTRA_MODE)
	{
		object_prep(q_ptr, lookup_kind(TV_AXE, SV_WEAPON_PICK));
		apply_magic(q_ptr,0,AM_FORCE_NORMAL);

		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		inven_carry(q_ptr);

	}





	//v1.1.37 小鈴はランダムな妖魔本を持っている
	if(p_ptr->pclass == CLASS_KOSUZU)
	{
		int j;
		for(j=0;j<3;j++)
		{
			monster_race *r_ptr;

			object_prep(q_ptr, lookup_kind(TV_CAPTURE, SV_CAPTURE_NONE));
			while (1)
			{
				i = randint1(max_r_idx - 1);
				r_ptr = &r_info[i];
				//レベル1以上20以下限定、人間不可、ユニーク不可
				if(!r_ptr->level || r_ptr->level > 20) continue;
				if(r_ptr->flags3 & RF3_HUMAN) continue;

				if (!item_monster_okay(i)) continue;
				if (!r_ptr->rarity) continue;

				break;
			}
			q_ptr->pval = i;
			q_ptr->xtra3 = get_mspeed(r_ptr);
			if (r_ptr->flags1 & RF1_FORCE_MAXHP) q_ptr->xtra4 = maxroll(r_ptr->hdice, r_ptr->hside);
			else	q_ptr->xtra4 = damroll(r_ptr->hdice, r_ptr->hside);
			q_ptr->xtra5 = q_ptr->xtra4;
			object_aware(q_ptr);
			object_known(q_ptr);
			q_ptr->ident |= (IDENT_MENTAL);
			inven_carry(q_ptr);
		}
	}

#if 0
	/* Give the player some food */
	switch (p_ptr->prace)
	{
	case RACE_VAMPIRE:
		/* Nothing! */
		/* Vampires can drain blood of creatures */
		break;

	case RACE_DEMON:
		/* Demon can drain vitality from humanoid corpse */

		/* Prepare allocation table */
		get_mon_num_prep(monster_hook_human, NULL);

		///del131221 death
		/*
		for (i = rand_range(3,4); i > 0; i--)
		{
			object_prep(q_ptr, lookup_kind(TV_CORPSE, SV_CORPSE));
			q_ptr->pval = get_mon_num(2);
			q_ptr->number = 1;
			add_outfit(q_ptr);
		}
		*/
		break;

#if 0
	case RACE_SKELETON:
		/* Some Skeletons */
		object_prep(q_ptr, lookup_kind(TV_SKELETON, SV_ANY));
		q_ptr->number = (byte)rand_range(7, 12);
		add_outfit(q_ptr);
		break;
#endif
	case RACE_SKELETON:
	case RACE_GOLEM:
	case RACE_ZOMBIE:
	case RACE_SPECTRE:
		/* Staff (of Nothing) */
		object_prep(q_ptr, lookup_kind(TV_STAFF, SV_STAFF_NOTHING));
		q_ptr->number = 1;

		add_outfit(q_ptr);
		break;

	case RACE_ENT:
		/* Potions of Water */
		object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_WATER));
		q_ptr->number = (byte)rand_range(15, 23);
		add_outfit(q_ptr);

		break;

	case RACE_ANDROID:
		/* Flasks of oil */
		object_prep(q_ptr, lookup_kind(TV_FLASK, SV_ANY));

		/* Fuel with oil (move pval to xtra4) */
		apply_magic(q_ptr, 1, AM_NO_FIXED_ART);

		q_ptr->number = (byte)rand_range(7, 12);
		add_outfit(q_ptr);

		break;

	default:
		/* Food rations */
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
		q_ptr->number = (byte)rand_range(3, 7);

		add_outfit(q_ptr);
	}

	/* Get local object */
	q_ptr = &forge;

	if ((p_ptr->prace == RACE_VAMPIRE) && (p_ptr->pclass != CLASS_NINJA))
	{
		/* Hack -- Give the player scrolls of DARKNESS! */
		object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_DARKNESS));

		q_ptr->number = (byte)rand_range(2, 5);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass != CLASS_NINJA)
	{
		/* Hack -- Give the player some torches */
		object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH));
		q_ptr->number = (byte)rand_range(3, 7);
		q_ptr->xtra4 = rand_range(3, 7) * 500;

		add_outfit(q_ptr);
	}

	/* Get local object */
	q_ptr = &forge;

	if ((p_ptr->pclass == CLASS_RANGER) || (p_ptr->pclass == CLASS_CAVALRY))
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL));
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	if (p_ptr->pclass == CLASS_RANGER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_BOW, SV_SHORT_BOW));

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_ARCHER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL));
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_HIGH_MAGE)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_WAND, SV_WAND_MAGIC_MISSILE));
		q_ptr->number = 1;
		q_ptr->pval = (byte)rand_range(25, 30);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_SORCERER)
	{
		for (i = TV_LIFE_BOOK; i <= TV_LIFE_BOOK+MAX_BASIC_MAGIC_REALM-1; i++)
		{
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(i, 0));
			q_ptr->number = 1;

			add_outfit(q_ptr);
		}
	}
	else if (p_ptr->pclass == CLASS_TOURIST)
	{
		if (p_ptr->pseikaku != SEIKAKU_SEXY)
		{
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(TV_SHOT, SV_AMMO_LIGHT));
			q_ptr->number = (byte)rand_range(15, 20);

			add_outfit(q_ptr);
		}

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_BISCUIT));
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_WAYBREAD));
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_JERKY));
		q_ptr->number = (byte)rand_range(1, 3);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_ALE));
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_WINE));
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_NINJA)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_SPIKE, 0));
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_SNIPER)
	{
		/* Hack -- Give the player some bolts */
		object_prep(q_ptr, lookup_kind(TV_BOLT, SV_AMMO_NORMAL));
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	///del131223
	/*
	if(p_ptr->pseikaku == SEIKAKU_SEXY)
	{
		player_init[p_ptr->pclass][2][0] = TV_HAFTED;
		player_init[p_ptr->pclass][2][1] = SV_WHIP;
	}
	*/
	/* Hack -- Give the player three useful objects */
	for (i = 0; i < 3; i++)
	{
		/* Look up standard equipment */
		tv = player_init[p_ptr->pclass][i][0];
		sv = player_init[p_ptr->pclass][i][1];

		if ((p_ptr->prace == RACE_ANDROID) && ((tv == TV_SOFT_ARMOR) || (tv == TV_HARD_ARMOR))) continue;
		/* Hack to initialize spellbooks */
		if (tv == TV_SORCERY_BOOK) tv = TV_LIFE_BOOK + p_ptr->realm1 - 1;
		else if (tv == TV_DEATH_BOOK) tv = TV_LIFE_BOOK + p_ptr->realm2 - 1;

		else if (tv == TV_RING && sv == SV_RING_RES_FEAR &&
		    p_ptr->prace == RACE_BARBARIAN)
			/* Barbarians do not need a ring of resist fear */
			sv = SV_RING_SUSTAIN_STR;

		else if (tv == TV_RING && sv == SV_RING_SUSTAIN_INT &&
		    p_ptr->prace == RACE_MIND_FLAYER)
		  {
			tv = TV_POTION;
			sv = SV_POTION_RESTORE_MANA;
		  }

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player an object */
		object_prep(q_ptr, lookup_kind(tv, sv));

		/* Assassins begin the game with a poisoned dagger */
		///item 初期所持アイテムをエゴ化している
		if ((tv == TV_SWORD || tv == TV_HAFTED) && (p_ptr->pclass == CLASS_ROGUE &&
			p_ptr->realm1 == REALM_DEATH)) /* Only assassins get a poisoned weapon */
		{
			q_ptr->name2 = EGO_BRAND_POIS;
		}

		add_outfit(q_ptr);
	}

	/* Hack -- make aware of the water */
	k_info[lookup_kind(TV_POTION, SV_POTION_WATER)].aware = TRUE;
#endif
}

#if 0
//v1.1.65 書き換えたので古いのを退避

/*
 * Player race
 */
/*:::p_ptr->praceを決定する*/
///mod140105 新種族テーブルに対応。未実装や初期選択不可のフラグが立ってる種族は表示しない。
static bool get_player_race(void)
{
	int     k, n, cs, os;
	cptr    str;
	char    c;
	char	sym[MAX_RACES];
	char    p2 = ')';
	char    buf[80], cur[80];

	//もしMAX_BIRTH_RACESを超えて種族を増やすときには、ここを構造体などに変えないとエラー起こす
	cptr help_index[MAX_BIRTH_RACES] =
	{"Human","Youkai","Half-Youkai","Sennin","Tennin","Fairy",
	"Kappa","Magician","Karasu-tengu","Hakurou-tengu","Oni","Death",
	"Warbeast","Tsukumo","Android","Gyokuto","Youko","Yamawaro",
	"Bake-danuki","Ningyo","Hofgoblin","Nyudo","Imp","Golem","Kobito","Zombie",
	"Vampire","Spectre","Stray-god","Daiyoukai","Demigod","Lunarian"};


	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("注意：《種族》によってキャラクターの先天的な資質やボーナスが変化します。", 23, 5);
#else
	put_str("Note: Your 'race' determines various intrinsic factors and bonuses.", 23 ,5);
#endif

	/* Dump races */
//	for (n = 0; n < MAX_RACES; n++)
	for (n = 0; n < MAX_BIRTH_RACES; n++)
	{
		/* Analyze */
		rp_ptr = &race_info[n];
		str = rp_ptr->title;

		/*:::この一文はMAX_RACEからMAX_BIRTH_RACEにすることで役に立たなくなったが念のため残しとく*/
		if(rp_ptr->flag_nobirth || rp_ptr->flag_special || rp_ptr->flag_only_unique) continue;
		/* Display */
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);
#ifdef JP
		sprintf(buf, "%c%c%s", sym[n], p2, str);
#else
		sprintf(buf, "%c%c %s", sym[n], p2, str);
#endif
		if(rp_ptr->flag_nofixed) c_put_str(TERM_L_DARK, buf, 12 + (n/5), 1 + 16 * (n%5));
		else put_str(buf, 12 + (n/5), 1 + 16 * (n%5));

	}

#ifdef JP
	sprintf(cur, "%c%c%s", '*', p2, "ランダム");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Choose */
	k = -1;
	cs = p_ptr->prace;
//	os = MAX_RACES;
	os = MAX_BIRTH_RACES;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			rp_ptr = &race_info[os];

			if(os != MAX_BIRTH_RACES && rp_ptr->flag_nofixed) c_put_str(TERM_L_DARK, cur, 12 + (os/5), 1 + 16 * (os%5));
			else c_put_str(TERM_WHITE, cur, 12 + (os/5), 1 + 16 * (os%5));
			put_str("                                   ", 3, 40);
//			if(cs == MAX_RACES)
			if(cs == MAX_BIRTH_RACES)
			{
#ifdef JP
				sprintf(cur, "%c%c%s", '*', p2, "ランダム");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("                                               ", 4, 40);
				put_str("                                               ", 5, 40);
			}
			else
			{
				rp_ptr = &race_info[cs];
				str = rp_ptr->title;
#ifdef JP
				sprintf(cur, "%c%c%s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rp_ptr->title, 3, 40);
				///mod140105 スコア倍率記述
				//put_str("の種族修正", 3, 40+strlen(rp_ptr->title));
				//put_str("腕力 知能 賢さ 器用 耐久 魅力 経験 ", 4, 40);
				put_str("の種族修正", 3, 40+strlen(rp_ptr->title));
				put_str("腕力 知能 賢さ 器用 耐久 魅力 経験  スコア倍率", 4, 40);
#else
				sprintf(cur, "%c%c %s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rp_ptr->title, 3, 40);
				put_str(": Race modification", 3, 40+strlen(rp_ptr->title));
				put_str("Str  Int  Wis  Dex  Con  Chr   EXP ", 4, 40);
#endif
				if(cs == RACE_ANDROID)
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d  **** %4d%% ",
					///mod140119 アンドロイドは経験値補正を表示しないようにした
					rp_ptr->r_adj[0], rp_ptr->r_adj[1], rp_ptr->r_adj[2], rp_ptr->r_adj[3],
					rp_ptr->r_adj[4], rp_ptr->r_adj[5], rp_ptr->score_mult);

				else
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %4d%% ",
					///mod140105 最後にスコア倍率記述を追加した
					rp_ptr->r_adj[0], rp_ptr->r_adj[1], rp_ptr->r_adj[2], rp_ptr->r_adj[3],
					rp_ptr->r_adj[4], rp_ptr->r_adj[5], (rp_ptr->r_exp - 100), rp_ptr->score_mult);


				c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/5), 1 + 16 * (cs%5));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		//sprintf(buf, "種族を選んで下さい (%c-%c) ('='初期オプション設定): ", sym[0], sym[MAX_RACES-1]);
		sprintf(buf, "種族を選んで下さい (%c-%c) ('='初期オプション設定): ", sym[0], sym[MAX_BIRTH_RACES-1]);
#else
		sprintf(buf, "Choose a race (%c-%c) ('=' for options): ", sym[0], sym[MAX_RACES-1]);
#endif

		put_str(buf, 10, 10);
		put_str("灰字の種族は未実装です。 ", 11, 10);

		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
//			if(cs == MAX_RACES)
			if(cs == MAX_BIRTH_RACES)//ランダム
			{
//				k = randint0(MAX_RACES);
				k = randint0(MAX_BIRTH_RACES);
				cs = k;
				continue;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
//			k = randint0(MAX_RACES);
			k = randint0(MAX_BIRTH_RACES);
			cs = k;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 5) cs -= 5;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
//			if (cs < MAX_RACES) cs++;
			if (cs < MAX_BIRTH_RACES) cs++;
		}
		if (c == '2')
		{
//			if ((cs + 5) <= MAX_RACES) cs += 5;
			if ((cs + 5) <= MAX_BIRTH_RACES) cs += 5;
		}
		k = (islower(c) ? A2I(c) : -1);
//		if ((k >= 0) && (k < MAX_RACES))
		if ((k >= 0) && (k < MAX_BIRTH_RACES))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
//		if ((k >= 26) && (k < MAX_RACES))
		if ((k >= 26) && (k < MAX_BIRTH_RACES))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?' && cs < MAX_BIRTH_RACES)
		{
			char help_entry[64];
			sprintf(help_entry,"traceclas.txt#%s",help_index[cs]);
#ifdef JP
			show_help(help_entry);
#else
			show_help("raceclas.txt#TheRaces");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif
			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set race */
	p_ptr->prace = k;

	rp_ptr = &race_info[p_ptr->prace];

	/* Display */
	c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);

	/* Success */
	return TRUE;
}
#endif


/*:::p_ptr->praceを決定する*/

//v1.1.65 MAX_BIRTH_RACES(32)を超える種族インデックスを扱えるよう改修
//race_info[]に記録されているクラスの中から初期選択可能なクラスをリスト表示し、選択を受け付けてそのインデックスをp_ptr->praceに格納して終了


static bool get_player_race(void)
{
	int     n;
	cptr    str;
	char    c;
	char	sym[MAX_RACES];
	char    p2 = ')';
	char    buf[80], cur[80];

	int	old_cur_pos = 0, new_cur_pos = 0;//古いカーソル位置と新しいカーソル位置　race_idx_list[]のインデックスに対応
	int	selecting_race_idx;


	int race_idx_list[MAX_RACES + 1], i;
	int valid_race_idx_count;//初期選択可能な種族数

					  //種族idxリストに初期選択可能な種族idxを格納していく
	valid_race_idx_count = 0;
	for (i = 0; i<MAX_RACES; i++)
	{

		//flag_nobirth(初期選択不可)、flag_special(特殊変身専用)、flag_only_unique(ユニーククラス専用)を弾く.
		//flag_nofixed(未実装)は後で処理する
		if (race_info[i].flag_nobirth || race_info[i].flag_special || race_info[i].flag_only_unique) continue;

		race_idx_list[valid_race_idx_count] = i;
		valid_race_idx_count++;

	}

	//リストの最後に「ランダム選択」を示すダミー種族idxとして-1を入れておく
	race_idx_list[valid_race_idx_count] = -1;


	//要らんと思うが念のため
	if (valid_race_idx_count > 52 || !valid_race_idx_count)
	{
		put_str("ラベルがa～Zまでで足りないかカウント値がおかしい", 23, 5);
		inkey();
		return FALSE;
	}

	/* Extra info */
	clear_from(10);
	put_str("注意：《種族》によってキャラクターの先天的な資質やボーナスが変化します。", 23, 5);

	//種族名をラベル付きで表示。「ランダム」はあとで描画する
	for (n = 0; n < valid_race_idx_count; n++)
	{
		/* Analyze */
		rp_ptr = &race_info[race_idx_list[n]];
		str = rp_ptr->title;

		//「新しいカーソル位置」に、現在設定されているpraceの表示位置をセットする。
		//種族選択後キャンセルしてもう一度この関数に来たとき、praceにはそのとき選択された種族idxが入っている。
		//全くの最初の場合player_wipe()により0(人間)が入っているはず。
		if (p_ptr->prace == race_idx_list[n]) new_cur_pos = n;

		/* Display */
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);
		sprintf(buf, "%c%c%s", sym[n], p2, str);

		if (rp_ptr->flag_nofixed)
			c_put_str(TERM_L_DARK, buf, 12 + (n / 5), 1 + 16 * (n % 5));
		else
			put_str(buf, 12 + (n / 5), 1 + 16 * (n % 5));

	}

	selecting_race_idx = -1;

	//old_cur_posには最初ランダム選択の場所を入れておく。
	//この後の「古いカーソル位置の色を戻すために描画し直す」処理でついでにまだ書かれていないランダム選択肢を描画するため。
	old_cur_pos = valid_race_idx_count;
	sprintf(cur, "%c%c%s", '*', p2, "ランダム");

	//キー入力を受け付け、方向キーが入力されたらカーソルを描画しそこの色を変えて元の場所の色を戻す
	while (1)
	{

		//古いrace_idxと新しいrace_idx
		int os = race_idx_list[old_cur_pos];
		int cs = race_idx_list[new_cur_pos];

		/* Move Cursol */
		if (old_cur_pos != new_cur_pos)
		{

			//前にカーソルがあった場所の再描画。未実装個所は暗くする。最初にこのループに入ったとき「ランダム」のエントリが初めて描画される
			if (os >= 0 && race_info[os].flag_nofixed)
				c_put_str(TERM_L_DARK, cur, 12 + (old_cur_pos / 5), 1 + 16 * (old_cur_pos % 5));
			else
				c_put_str(TERM_WHITE, cur, 12 + (old_cur_pos / 5), 1 + 16 * (old_cur_pos % 5));

			//画面上の各種族パラメータ情報欄と現在カーソルが当たっている場所の再描画
			put_str("                                   ", 3, 40);
			if (cs < 0)
			{
				sprintf(cur, "%c%c%s", '*', p2, "ランダム");
				put_str("                                               ", 4, 40);
				put_str("                                               ", 5, 40);
			}
			else
			{
				rp_ptr = &race_info[cs];
				str = rp_ptr->title;
				sprintf(cur, "%c%c%s", sym[new_cur_pos], p2, str);
				c_put_str(TERM_L_BLUE, rp_ptr->title, 3, 40);
				///mod140105 スコア倍率記述
				put_str("の種族修正", 3, 40 + strlen(rp_ptr->title));
				put_str("腕力 知能 賢さ 器用 耐久 魅力 経験  スコア倍率", 4, 40);

				if (cs == RACE_ANDROID)
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d  **** %4d%% ",
						///mod140119 アンドロイドは経験値補正を表示しないようにした
						rp_ptr->r_adj[0], rp_ptr->r_adj[1], rp_ptr->r_adj[2], rp_ptr->r_adj[3],
						rp_ptr->r_adj[4], rp_ptr->r_adj[5], rp_ptr->score_mult);

				else
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %4d%% ",
						///mod140105 最後にスコア倍率記述を追加した
						rp_ptr->r_adj[0], rp_ptr->r_adj[1], rp_ptr->r_adj[2], rp_ptr->r_adj[3],
						rp_ptr->r_adj[4], rp_ptr->r_adj[5], (rp_ptr->r_exp - 100), rp_ptr->score_mult);


				c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 12 + (new_cur_pos / 5), 1 + 16 * (new_cur_pos % 5));
			old_cur_pos = new_cur_pos;
		}

		if (selecting_race_idx >= 0) break;

		sprintf(buf, "種族を選んで下さい (%c-%c) ('='初期オプション設定): ", sym[0], sym[valid_race_idx_count - 1]);

		put_str(buf, 10, 10);
		put_str("灰字の種族は未実装です。 ", 11, 10);

		//入力されたキーによって種族選択処理したりカーソル動かす処理したり分岐
		c = inkey();

		if (c == 'Q')
		{
			birth_quit();
		}
		else if (c == 'S' || c == ESCAPE)
		{
			return (FALSE);
		}
		else if (c == ' ' || c == '\r' || c == '\n')
		{
			if (race_idx_list[new_cur_pos] < 0)//ランダムのときはランダムな値を選択されたidxとし、選択処理だけでなくループを一周回ってカーソル再描画してから終了
			{
				new_cur_pos = randint0(valid_race_idx_count);
				selecting_race_idx = race_idx_list[new_cur_pos];
			}
			else	//カーソル位置で直接リターンを押されたらそのまま選択処理し終了
			{
				selecting_race_idx = race_idx_list[new_cur_pos];
				break;
			}
		}
		else if (c == '*')
		{
			new_cur_pos = randint0(valid_race_idx_count);
			selecting_race_idx = race_idx_list[new_cur_pos];
		}
		//方向キーによるカーソル位置情報移動
		else if (c == '8')
		{
			if (new_cur_pos >= 5) new_cur_pos -= 5;
		}
		else if (c == '4')
		{
			if (new_cur_pos > 0) new_cur_pos--;
		}
		else if (c == '6')
		{
			if (new_cur_pos < valid_race_idx_count) new_cur_pos++;
		}
		else if (c == '2')
		{
			if ((new_cur_pos + 5) <= valid_race_idx_count) new_cur_pos += 5;
		}
		//アルファベット入力によるカーソル位置情報移動と選択処理
		else if (islower(c) && A2I(c) < valid_race_idx_count)
		{
			new_cur_pos = A2I(c);
			selecting_race_idx = race_idx_list[new_cur_pos];
			continue;
		}
		else if (isupper(c) && (c - 'A' + 26) < valid_race_idx_count)
		{
			new_cur_pos = (c - 'A' + 26);
			selecting_race_idx = race_idx_list[new_cur_pos];
			continue;
		}
		//カーソルがある位置のヘルプファイル表示
		else if (c == '?' && new_cur_pos < valid_race_idx_count )
		{
			char help_entry[64];

			rp_ptr = &race_info[cs];
			sprintf(help_entry, "traceclas.txt#%s", rp_ptr->E_title);
			show_help(help_entry);

		}
		else if (c == '=')
		{
			screen_save();
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
			screen_load();
		}
		else
		{
			bell();

		}

	}

	/* Set race */
	p_ptr->prace = selecting_race_idx;

	rp_ptr = &race_info[p_ptr->prace];

	/* Display */
	c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);

	/* Success */
	return TRUE;
}






/*
 * Player class
 */
static bool get_player_class(void)
{
	int     k, n, cs, os,i;
	char    c;
	char	sym[MAX_CLASS];
	char    p2 = ')';
	char    buf[80], cur[80];
	cptr    str;

	int class_list_len=0;
	int class_list[MAX_CLASS];
	int class_idx;

	/* v2.0.19 class_info[]の英職業名を使うことにした
	cptr help_index[MAX_CLASS_CHOICE+1] =
	{"Warrior","Mage","Priest","Explorer","Ranger","Paladin",
	"Teacher","Maid","Martial-Artist","Mindcrafter","High-Mage","Civilian",
	"Syugen","Magic-Knight","Bullet-Researcher","Archer","Magic-Eater","Engineer",
	"Librarian","Samurai","Soldier","Chemist","Cavalry","Tsukumo-Master","Secondhand-dealer","Jeweler",
	"Ninja", "Card-Dealer",""};
	*/

	//v2.0.19 一般職リストを作成する
	for (i = 0; i < MAX_CLASS; i++)
	{
		if (class_info[i].flag_only_unique) continue;
		if (i == CLASS_OUTSIDER) continue;

		class_list[class_list_len] = i;

		class_list_len++;
	}


	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("注意：《職業》によってキャラクターの先天的な能力やボーナスが変化します。", 23, 5);
#else
	put_str("Note: Your 'class' determines various intrinsic abilities and bonuses.", 23, 5);
#endif

#ifdef JP
	//put_str("()で囲まれた選択肢はこの種族には似合わない職業です。", 11, 10);
	put_str("灰字の職業は未実装です。", 11, 10);
#else
	put_str("Any entries in parentheses should only be used by advanced players.", 11, 5);
#endif


	/* Dump classes */
	for (n = 0; n < class_list_len; n++)
	{
		class_idx = class_list[n];

		/* Analyze */
		cp_ptr = &class_info[class_idx];

		///mod140202 性別ごとに職業名を分けた
		if(p_ptr->psex == SEX_MALE)	str = cp_ptr->title;
		else						str = cp_ptr->f_title;

		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);

		/*:::向いてない職業には()をつける。現在は実質停止中*/
		/* Display */
		/*
		if (!(rp_ptr->choice & (1L << n)))
#ifdef JP
			sprintf(buf, "%c%c(%s)", sym[n], p2, str);
#else
			sprintf(buf, "%c%c (%s)", sym[n], p2, str);
#endif
		else
#ifdef JP
			sprintf(buf, "%c%c%s", sym[n], p2, str);
#else
			sprintf(buf, "%c%c %s", sym[n], p2, str);
#endif
		*/

		sprintf(buf, "%c%c%s", sym[n], p2, str);

		if(cp_ptr->flag_nofixed) c_put_str(TERM_L_DARK, buf, 13 + (n/4), 2 + 19 * (n%4));
		else put_str(buf, 13+ (n/4), 2 + 19 * (n%4));
	}

#ifdef JP
	sprintf(cur, "%c%c%s", '*', p2, "ランダム");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Get a class */
	k = -1;
//	cs = p_ptr->pclass;
//	os = MAX_CLASS_CHOICE;
	cs = 0;
	os = class_list_len;
	while (1)
	{
		class_idx = class_list[cs];
		/* Move Cursol */
		if (cs != os)
		{
			if(os != class_list_len && cp_ptr->flag_nofixed) c_put_str(TERM_L_DARK, cur, 13 + (os/4), 2 + 19 * (os%4));
			else c_put_str(TERM_WHITE, cur, 13 + (os/4), 2 + 19 * (os%4));
			put_str("                                   ", 3, 40);
			if(cs == class_list_len)
			{
#ifdef JP
				sprintf(cur, "%c%c%s", '*', p2, "ランダム");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("                                               ", 4, 40);
				put_str("                                               ", 5, 40);
			}
			else
			{
				int mul_score;
				cp_ptr = &class_info[class_idx];
				///mod140202 性別ごとに職業名を分けた
				if(p_ptr->psex == SEX_MALE)	str = cp_ptr->title;
				else						str = cp_ptr->f_title;

/*種族に向かない職業に()をつける本家の仕様を無効化する
				if (!(rp_ptr->choice & (1L << cs)))
#ifdef JP
					sprintf(cur, "%c%c(%s)", sym[cs], p2, str);
#else
					sprintf(cur, "%c%c (%s)", sym[cs], p2, str);
#endif
				else
#ifdef JP
					sprintf(cur, "%c%c%s", sym[cs], p2, str);
#else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
#endif
*/
					sprintf(cur, "%c%c%s", sym[cs], p2, str);

#ifdef JP
					//c_put_str(TERM_L_BLUE, cp_ptr->title, 3, 40);
					if(p_ptr->psex == SEX_MALE)	c_put_str(TERM_L_BLUE, cp_ptr->title, 3, 40);
					else						c_put_str(TERM_L_BLUE, cp_ptr->f_title, 3, 40);

					if(p_ptr->psex == SEX_MALE)	put_str("の職業修正", 3, 40+strlen(cp_ptr->title));
					else						put_str("の職業修正", 3, 40+strlen(cp_ptr->f_title));
					//put_str("の職業修正", 3, 40+strlen(cp_ptr->title));
					put_str("腕力 知能 賢さ 器用 耐久 魅力 経験 スコア", 4, 40);
#else
					c_put_str(TERM_L_BLUE, cp_ptr->title, 3, 40);
					put_str(": Class modification", 3, 40+strlen(cp_ptr->title));
					put_str("Str  Int  Wis  Dex  Con  Chr   EXP ", 4, 40);
#endif
					mul_score = cp_ptr->score_mult;
					extra_mode_score_mult(&mul_score,class_idx); 
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %3d%% ",
						cp_ptr->c_adj[0], cp_ptr->c_adj[1], cp_ptr->c_adj[2], cp_ptr->c_adj[3],
						cp_ptr->c_adj[4], cp_ptr->c_adj[5], cp_ptr->c_exp, mul_score);
					c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 13 + (cs/4), 2 + 19 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "職業を選んで下さい (%c-%c) ('='初期オプション設定): ", sym[0], sym[class_list_len-1]);
#else
		sprintf(buf, "Choose a class (%c-%c) ('=' for options): ",  sym[0], sym[MAX_CLASS_CHOICE-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == class_list_len)
			{
				int tmp = randint0(class_list_len);
				k = class_list[tmp];
				cs = tmp;
				continue;
			}
			else
			{
				k = class_list[cs];
				break;
			}
		}
		if (c == '*')
		{
			int tmp = randint0(class_list_len);
			k = class_list[tmp];
			cs = tmp;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < class_list_len) cs++;
		}
		if (c == '2')
		{
			if ((cs + 4) <= class_list_len) cs += 4;
		}

		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < class_list_len))
		{
			cs = k;
			continue;
		}

		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < class_list_len))
		{
			cs = k;
			continue;
		}
		else k = -1;




		if (c == '?' && cs < class_list_len)
		{
			char help_entry[64];
//			sprintf(help_entry,"traceclas.txt#%s",help_index[cs]);
			sprintf(help_entry,"traceclas.txt#%s",class_info[class_idx].E_title);
#ifdef JP
			show_help(help_entry);
#else
			show_help("raceclas.txt#TheRaces");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set class */
	p_ptr->pclass = class_idx;
	cp_ptr = &class_info[p_ptr->pclass];
	//mp_ptr = &m_info[p_ptr->pclass];

	if(p_ptr->psex == SEX_MALE )strcpy(player_class_name, cp_ptr->title);
	else strcpy(player_class_name, cp_ptr->f_title);

	/* Display */
	//c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 15);
	if(p_ptr->psex == SEX_MALE)	c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 15);
	else						c_put_str(TERM_L_BLUE, cp_ptr->f_title, 5, 15);



	return TRUE;
}


/*
 * Player seikaku
 */
static bool get_player_seikaku(void)
{
	int     k, n, os, cs, term;
	char    c;
	char	sym[MAX_SEIKAKU];
	char    p2 = ')';
	char    buf[80], cur[80];
	char    tmp[64];
	cptr    str;


	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("注意：《性格》によってキャラクターの能力やボーナスが変化します。", 23, 5);
	put_str("灰色の性格は未実装です。", 24, 6);
#else
	put_str("Note: Your personality determines various intrinsic abilities and bonuses.", 23, 5);
#endif

	/* Dump seikakus */
	for (n = 0; n < MAX_SEIKAKU; n++)
	{

		/* Analyze */
		//v1.1.32 特殊性格実装
		//ap_ptr = &seikaku_info[n];
		ap_ptr = get_ap_ptr(p_ptr->prace,p_ptr->pclass,n);
		if(p_ptr->psex == SEX_MALE) str = ap_ptr->title;
		else  str = ap_ptr->f_title;
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);

		/* Display */
		/* Display */
#ifdef JP
		sprintf(buf, "%c%c%s", I2A(n), p2, str);
#else
		sprintf(buf, "%c%c %s", I2A(n), p2, str);
#endif
		if(ap_ptr->flag_nofixed) term = TERM_L_DARK;
		else term = TERM_WHITE;
		c_put_str(term, buf, 12 + (n/4), 2 + 18 * (n%4));
	}

#ifdef JP
	sprintf(cur, "%c%c%s", '*', p2, "ランダム");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Get a seikaku */
	k = -1;
	cs = p_ptr->pseikaku;
	os = MAX_SEIKAKU;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			if(os != MAX_SEIKAKU && ap_ptr->flag_nofixed) term = TERM_L_DARK;
			else term = TERM_WHITE;
			c_put_str(term, cur, 12 + (os/4), 2 + 18 * (os%4));
			//c_put_str(TERM_WHITE, cur, 12 + (os/4), 2 + 18 * (os%4));
			put_str("                                             ", 3, 40);
			if(cs == MAX_SEIKAKU)
			{
#ifdef JP
				sprintf(cur, "%c%c%s", '*', p2, "ランダム");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("                                           ", 4, 40);
				put_str("                                           ", 5, 40);
			}
			else
			{
				//v1.1.32 特殊性格実装
				//ap_ptr = &seikaku_info[cs];
				ap_ptr = get_ap_ptr(p_ptr->prace,p_ptr->pclass,cs);
				if(p_ptr->psex == SEX_MALE) str = ap_ptr->title;
				else  str = ap_ptr->f_title;
#ifdef JP
					sprintf(cur, "%c%c%s", sym[cs], p2, str);
#else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
#endif
#ifdef JP
				if(p_ptr->psex == SEX_MALE)		
					c_put_str(TERM_L_BLUE, ap_ptr->title, 3, 40);
				else  			
					c_put_str(TERM_L_BLUE, ap_ptr->f_title, 3, 40);
				if(p_ptr->psex == SEX_MALE) 
					put_str("の性格修正", 3, 40+strlen(ap_ptr->title));
				else  			
					put_str("の性格修正", 3, 40+strlen(ap_ptr->f_title));

					put_str("腕力 知能 賢さ 器用 耐久 魅力      ", 4, 40);
#else
					c_put_str(TERM_L_BLUE, ap_ptr->title, 3, 40);
					put_str(": Personality modification", 3, 40+strlen(ap_ptr->title));
					put_str("Str  Int  Wis  Dex  Con  Chr       ", 4, 40);
#endif
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d       ",
						ap_ptr->a_adj[0], ap_ptr->a_adj[1], ap_ptr->a_adj[2], ap_ptr->a_adj[3],
						ap_ptr->a_adj[4], ap_ptr->a_adj[5]);
					c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/4), 2 + 18 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "性格を選んで下さい (%c-%c) ('='初期オプション設定): ", sym[0], sym[MAX_SEIKAKU-1]);
#else
		sprintf(buf, "Choose a personality (%c-%c) ('=' for options): ", sym[0], sym[MAX_SEIKAKU-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == MAX_SEIKAKU)
			{

				k = randint0(MAX_SEIKAKU);

				cs = k;
				continue;
			}
			else
			{

				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			k = randint0(n);
			cs = k;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;

		}
		if (c == '4')
		{
			if (cs > 0) cs--;

		}
		if (c == '6')
		{
			if (cs < MAX_SEIKAKU) cs++;

		}
		if (c == '2')
		{
			if ((cs + 4) <= MAX_SEIKAKU) cs += 4;

		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < MAX_SEIKAKU))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < MAX_SEIKAKU))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("traceclas.txt#ThePersonalities");
#else
			show_help("raceclas.txt#ThePersonalities");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set seikaku */
	//v1.1.32 特殊性格実装
	//ap_ptr = &seikaku_info[p_ptr->pseikaku];
	ap_ptr = get_ap_ptr(p_ptr->prace,p_ptr->pclass,k);
	p_ptr->pseikaku = k;
#ifdef JP
	if(p_ptr->psex == SEX_MALE)		
		strcpy(tmp, ap_ptr->title);
	else
		strcpy(tmp, ap_ptr->f_title);
	if(ap_ptr->no == 1)
	strcat(tmp,"の");
#else
	strcpy(tmp, ap_ptr->title);
	strcat(tmp," ");
#endif
	strcat(tmp,player_name);


	/* Display */
	//c_put_str(TERM_L_BLUE, tmp, 1, 34);
	c_put_str(TERM_L_BLUE, tmp, 1, 24);

	return TRUE;
}

/*:::オートローラー　最低限得たい値をstat_limit[]に設定する*/
#ifdef ALLOW_AUTOROLLER
static bool get_stat_limits(void)
{
	int i, j, m, cs, os;
	int cval[6];
	char c;
	char buf[80], cur[80];
	char inp[80];

	/* Clean up */
	clear_from(10);

	/* Extra infomation */
#ifdef JP
	put_str("最低限得たい能力値を設定して下さい。", 10, 10);
	put_str("2/8で項目選択、4/6で値の増減、Enterで次へ", 11, 10);
#else
	put_str("Set minimum stats.", 10, 10);
	put_str("2/8 for Select, 4/6 for Change value, Enter for Goto next", 11, 10);
#endif
	
#ifdef JP
	put_str("         基本値  種族 職業 性格     合計値  最大値", 13, 10);
#else
	put_str("           Base   Rac  Cla  Per      Total  Maximum", 13, 10);
#endif

	/* Output the maximum stats */
	for (i = 0; i < 6; i++)
	{
		/* Reset the "success" counter */
		stat_match[i] = 0;
		cval[i] = 3;

		/* Race/Class bonus */
		j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i];

		/* Obtain the "maximal" stat */
		m = adjust_stat(17, j);
///mod140104 パラメータ上限と表記を変更に伴い表記変更
#if 0
		/* Above 18 */
		if (m > 18)
		{
#ifdef JP
			sprintf(cur, "18/%02d", (m - 18));
#else
			sprintf(cur, "18/%02d", (m - 18));
#endif
		}
		
		/* From 3 to 18 */
		else
#endif
		{
#ifdef JP
			sprintf(cur, "%2d", m);
#else
			sprintf(cur, "%2d", m);
#endif
		}

		/* Obtain the current stat */
		m = adjust_stat(cval[i], j);
#if 0
		/* Above 18 */
		if (m > 18)
		{
#ifdef JP
			sprintf(inp, "18/%02d", (m - 18));
#else
			sprintf(inp, "18/%02d", (m - 18));
#endif
		}
		
		/* From 3 to 18 */
		else
#endif
		{
#ifdef JP
			sprintf(inp, "%2d", m);
#else
			sprintf(inp, "%2d", m);
#endif
		}

		/* Prepare a prompt */
		sprintf(buf, "%6s       %2d   %+3d  %+3d  %+3d  =  %6s  %6s",
			stat_names[i], cval[i], rp_ptr->r_adj[i], cp_ptr->c_adj[i],
			ap_ptr->a_adj[i], inp, cur);
		
		/* Dump the prompt */
		put_str(buf, 14 + i, 10);
	}
	
	/* Get a minimum stat */
	cs = 0;
	os = 6;
	while (TRUE)
	{
		/* Move Cursol */
		if (cs != os)
		{
			if(os == 6)
			{
#ifdef JP
				c_put_str(TERM_WHITE, "決定する", 21, 35);
#else
				c_put_str(TERM_WHITE, "Accept", 21, 35);
#endif
			}
			else if(os < 6)
				c_put_str(TERM_WHITE, cur, 14 + os, 10);
			
			if(cs == 6)
			{
#ifdef JP
				c_put_str(TERM_YELLOW, "決定する", 21, 35);
#else
				c_put_str(TERM_YELLOW, "Accept", 21, 35);
#endif
			}
			else
			{
				/* Race/Class bonus */
				j = rp_ptr->r_adj[cs] + cp_ptr->c_adj[cs] + ap_ptr->a_adj[cs];

				/* Obtain the current stat */
				m = adjust_stat(cval[cs], j);
#if 0				
				/* Above 18 */
				if (m > 18)
				{
#ifdef JP
					sprintf(inp, "18/%02d", (m - 18));
#else
					sprintf(inp, "18/%02d", (m - 18));
#endif
				}
				
				/* From 3 to 18 */
				else
#endif
				{
#ifdef JP
					sprintf(inp, "%2d", m);
#else
					sprintf(inp, "%2d", m);
#endif
				}
				
				/* Prepare a prompt */
				sprintf(cur, "%6s       %2d   %+3d  %+3d  %+3d  =  %6s",
					stat_names[cs], cval[cs], rp_ptr->r_adj[cs],
					cp_ptr->c_adj[cs], ap_ptr->a_adj[cs], inp);
				c_put_str(TERM_YELLOW, cur, 14 + cs, 10);
			}
			os = cs;
		}
		
		/* Prompt for the minimum stats */
		c = inkey();
		switch ( c ){
		case 'Q':
			birth_quit();
		case 'S':
			return FALSE;
		case ESCAPE:
			break;
		case ' ':
		case '\r':
		case '\n':
			if(cs == 6) break;
			cs++;
			c = '2';
			break;
		case '8':
		case 'k':
			if (cs > 0) cs--;
			break;
		case '2':
		case 'j':
			if (cs < 6) cs++;
			break;
		case '4':
		case 'h':
			if (cs != 6)
			{
				if (cval[cs] == 3)
				{
					cval[cs] = 17;
					os = 7;
				}
				else if (cval[cs] > 3)
				{
					cval[cs]--;
					os = 7;
				}
				else return FALSE;
			}
			break;
		case '6':
		case 'l':
			if (cs != 6)
			{
				if (cval[cs] == 17)
				{
					cval[cs] = 3;
					os = 7;
				}
				else if (cval[cs] < 17)
				{
					cval[cs]++;
					os = 7;
				}
				else return FALSE;
			}
			break;
		case 'm':
			if(cs != 6)
			{
				cval[cs] = 17;
				os = 7;
			}
			break;
		case 'n':
			if(cs != 6)
			{
				cval[cs] = 3;
				os = 7;
			}
			break;
		case '?':
#ifdef JP
			show_help("jbirth.txt#AutoRoller");
#else
			show_help("birth.txt#AutoRoller");
#endif
			break;
		case '=':
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
			break;
		default:
			bell();
			break;
		}
		if(c == ESCAPE || ((c == ' ' || c == '\r' || c == '\n') && cs == 6))break;
	}
	
	for (i = 0; i < 6; i++)
	{
		/* Save the minimum stat */
		stat_limit[i] = cval[i];
	}

	return TRUE;
}
#endif


#define STAT_DEFAULT 10

static bool get_bonus_points(void)
{
	int i, j, m, cs, os;
	int bonus_points;
	int bonus_use_total=0;
	char c;
	char buf[80];
	int bonus_use[6],stat_sum[6];

	/* Clean up */
	clear_from(10);

	if(ironman_no_bonus) bonus_points = 0;
	else if(birth_bonus_10) bonus_points = 10;
	else if(birth_bonus_20) bonus_points = 20;
	else
	{
		bonus_points = damroll(3,6);
		while(1)
		{
			if(one_in_(3)) break;
			bonus_points++;
		}
		if(one_in_(64)) bonus_points += 10;
	}
	if(difficulty == DIFFICULTY_EASY)  bonus_points += 10;
	if(bonus_points > 99) bonus_points = 99;

	put_str("能力値にボーナスポイントを割り振って下さい。", 10, 10);
	put_str("2/8で項目選択、4/6で値の増減", 11, 10);
	put_str("         基本値  種族 職業 性格   ボーナス  合計値", 13, 10);

	for (i = 0; i < 6; i++)
		bonus_use[i] = 0;

	cs = 0;
	while(1)
	{
		for (i = 0; i < 6; i++)
		{
			stat_sum[i] = STAT_DEFAULT + rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i] + bonus_use[i];

			sprintf(buf, "%6s       %2d   %+2d   %+2d   %+2d   + %3d   =   %3d",
				stat_names[i], STAT_DEFAULT, rp_ptr->r_adj[i], cp_ptr->c_adj[i],	ap_ptr->a_adj[i], bonus_use[i], stat_sum[i]);
			if(cs == i) c_put_str(TERM_YELLOW, buf, 14 + i, 10);
			else		c_put_str(TERM_WHITE , buf, 14 + i, 10);
		}
		if(cs == 6) c_put_str(TERM_YELLOW, "決定する", 21, 35);
		else c_put_str(TERM_WHITE, "決定する", 21, 35);
		sprintf(buf,"(残りボーナスポイント：%d)",bonus_points - bonus_use_total);
		c_put_str(TERM_WHITE, buf, 22, 35);

		c = inkey();
		switch ( c ){
		case 'Q':
			birth_quit();
		case 'S':
			return FALSE;
		case ESCAPE:
			break;
		case ' ':
		case '\r':
		case '\n':
			if(cs == 6) break;
			cs++;
			c = '2';
			break;
		case '8':
		case 'k':
			if (cs > 0) cs--;
			break;
		case '2':
		case 'j':
			if (cs < 6) cs++;
			break;
		case '4':
		case 'h':
			if (cs != 6 && bonus_use[cs] != 0)
			{
				bonus_use[cs]--;
				bonus_use_total--;
			}
			break;
		case '6':
		case 'l':
			if (cs != 6 && (bonus_use[cs] + STAT_DEFAULT < STAT_MAX_MAX) && bonus_use_total < bonus_points)
			{
				bonus_use[cs]++;
				bonus_use_total++;
			}
			break;
			///mod150103 ボーナスポイントオプション実装のためボーナスポイント振り分け画面からオプション設定できなくした
/*
		case '=':
			screen_save();
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
			screen_load();
			break;
*/
		case '?':
			show_help("tbirth.txt#BonusPoint");
			break;

		default:
			bell();
			break;
		}
		if(c == ESCAPE || ((c == ' ' || c == '\r' || c == '\n') && cs == 6))break;
	}
	
	for (i = 0; i < 6; i++)
	{
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = STAT_DEFAULT + bonus_use[i];
	}
	return TRUE;
}


#ifdef ALLOW_AUTOROLLER
static bool get_chara_limits(void)
{
#define MAXITEMS 8

	int i, j, m, cs, os;
	int mval[MAXITEMS], cval[MAXITEMS];
	int max_percent, min_percent;
	char c;
	char buf[80], cur[80];
	cptr itemname[] = {
#ifdef JP
		"年齢",
		"身長(インチ)",
		"体重(ポンド)",
		"社会的地位"
#else
		"age",
		"height",
		"weight",
		"social class"
#endif
	};

	/* Clean up */
	clear_from(10);
	
	/* Prompt for the minimum stats */
#ifdef JP
	put_str("2/4/6/8で項目選択、+/-で値の増減、Enterで次へ", 11, 10);
	put_str("注意：身長と体重の最大値/最小値ぎりぎりの値は非常に出現確率が低くなります。", 23, 2);
#else
	put_str("2/4/6/8 for Select, +/- for Change value, Enter for Goto next", 11, 10);
	put_str("Caution: Values near minimum or maximum is extremery rare.", 23, 5);
#endif
	
	if (p_ptr->psex == SEX_MALE)
	{
		max_percent = (int)(rp_ptr->m_b_ht+rp_ptr->m_m_ht*4-1) * 100 / (int)(rp_ptr->m_b_ht);
		min_percent = (int)(rp_ptr->m_b_ht-rp_ptr->m_m_ht*4+1) * 100 / (int)(rp_ptr->m_b_ht);
	}
	else
	{
		max_percent = (int)(rp_ptr->f_b_ht+rp_ptr->f_m_ht*4-1) * 100 / (int)(rp_ptr->f_b_ht);
		min_percent = (int)(rp_ptr->f_b_ht-rp_ptr->f_m_ht*4+1) * 100 / (int)(rp_ptr->f_b_ht);
	}
	
#ifdef JP
	put_str("体格/地位の最小値/最大値を設定して下さい。", 10, 10);
	put_str("  項    目                 最小値  最大値", 13,20);
#else
	put_str(" Parameter                    Min     Max", 13,20);
	put_str("Set minimum/maximum attribute.", 10, 10);
#endif

	/* Output the maximum stats */
	for (i = 0; i < MAXITEMS; i++)
	{
		/* Obtain the "maximal" stat */
		switch (i)
		{
		case 0:	/* Minimum age */
			m = rp_ptr->b_age + 1;
			break;
		case 1:	/* Maximum age */
			m = rp_ptr->b_age + rp_ptr->m_age;
			break;

		case 2:	/* Minimum height */
			if (p_ptr->psex == SEX_MALE) m = rp_ptr->m_b_ht-rp_ptr->m_m_ht*4+1;
			else m = rp_ptr->f_b_ht-rp_ptr->f_m_ht*4+1;
			break;
		case 3:	/* Maximum height */
			if (p_ptr->psex == SEX_MALE) m = rp_ptr->m_b_ht+rp_ptr->m_m_ht*4-1;
			else m = rp_ptr->f_b_ht+rp_ptr->f_m_ht*4-1;
			break;
		case 4:	/* Minimum weight */
			if (p_ptr->psex == SEX_MALE) m = (rp_ptr->m_b_wt * min_percent / 100) - (rp_ptr->m_m_wt * min_percent / 75) +1;
			else m = (rp_ptr->f_b_wt * min_percent / 100) - (rp_ptr->f_m_wt * min_percent / 75) +1;
			break;
		case 5:	/* Maximum weight */
			if (p_ptr->psex == SEX_MALE) m = (rp_ptr->m_b_wt * max_percent / 100) + (rp_ptr->m_m_wt * max_percent / 75) -1;
			else m = (rp_ptr->f_b_wt * max_percent / 100) + (rp_ptr->f_m_wt * max_percent / 75) -1;
			break;
		case 6:	/* Minimum social class */
			m = 1;
			break;
		case 7:	/* Maximum social class */
			m = 100;
			break;
		default:
			m = 1;
			break;
		}
		
		/* Save the maximum or minimum */
		mval[i] = m;
		cval[i] = m;
	}

	for (i = 0; i < 4; i++)
	{
		/* Prepare a prompt */
		sprintf(buf, "%-12s (%3d - %3d)", itemname[i], mval[i*2], mval[i*2+1]);

		/* Dump the prompt */
		put_str(buf, 14 + i, 20);

		for (j = 0; j < 2; j++)
		{
			sprintf(buf, "     %3d", cval[i*2+j]);
			put_str(buf, 14 + i, 45 + 8 * j);
		}
	}
	
	/* Get a minimum stat */
	cs = 0;
	os = MAXITEMS;
	while (TRUE)
	{
		/* Move Cursol */
		if (cs != os)
		{
#ifdef JP
			const char accept[] = "決定する";
#else
			const char accept[] = "Accept";
#endif
			if(os == MAXITEMS)
			{
				c_put_str(TERM_WHITE, accept, 19, 35);
			}
			else
			{
				c_put_str(TERM_WHITE, cur, 14 + os/2, 45 + 8 * (os%2));
			}
			
			if(cs == MAXITEMS)
			{
				c_put_str(TERM_YELLOW, accept, 19, 35);
			}
			else
			{
				/* Prepare a prompt */
				sprintf(cur, "     %3d", cval[cs]);
				c_put_str(TERM_YELLOW, cur, 14 + cs/2, 45 + 8 * (cs%2));
			}
			os = cs;
		}
		
		/* Prompt for the minimum stats */
		c = inkey();
		switch (c){
		case 'Q':
			birth_quit();
		case 'S':
			return (FALSE);
		case ESCAPE:
			break; /*後でもう一回breakせんと*/
		case ' ':
		case '\r':
		case '\n':
			if(cs == MAXITEMS) break;
			cs++;
			c = '6';
			break;
		case '8':
		case 'k':
			if (cs-2 >= 0) cs -= 2;
			break;
		case '2':
		case 'j':
			if (cs < MAXITEMS) cs += 2;
			if (cs > MAXITEMS) cs = MAXITEMS;
			break;
		case '4':
		case 'h':
			if (cs > 0) cs--;
			break;
		case '6':
		case 'l':
			if (cs < MAXITEMS) cs++;
			break;
		case '-':
		case '<':
			if (cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] > cval[cs-1])
					{
						cval[cs]--;
						os = 127;
					}
				}
				else
				{
					if(cval[cs] > mval[cs])
					{
						cval[cs]--;
						os = 127;
					}
				}
			}
			break;
		case '+':
		case '>':
			if (cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] < mval[cs])
					{
						cval[cs]++;
						os = 127;
					}
				}
				else
				{
					if(cval[cs] < cval[cs+1])
					{
						cval[cs]++;
						os = 127;
					}
				}
			}
			break;
		case 'm':
			if(cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] < mval[cs])
					{
						cval[cs] = mval[cs];
						os = 127;
					}
				}
				else
				{
					if(cval[cs] < cval[cs+1])
					{
						cval[cs] = cval[cs+1];
						os = 127;
					}
				}
			}
			break;
		case 'n':
			if(cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] > cval[cs-1])
					{
						cval[cs] = cval[cs-1];
						os = 255;
					}
				}
				else
				{
					if(cval[cs] > mval[cs])
					{
						cval[cs] = mval[cs];
						os = 255;
					}
				}
			}
			break;
		case '?':
#ifdef JP
			show_help("jbirth.txt#AutoRoller");
#else
			show_help("birth.txt#AutoRoller");
#endif
			break;
		case '=':
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
			break;
		default:
			bell();
			break;
		}
		if(c == ESCAPE || ((c == ' ' || c == '\r' || c == '\n') && cs == MAXITEMS))break;
	}

	/* Input the minimum stats */
	chara_limit.agemin = cval[0];
	chara_limit.agemax = cval[1];
	chara_limit.htmin = cval[2];
	chara_limit.htmax = cval[3];
	chara_limit.wtmin = cval[4];
	chara_limit.wtmax = cval[5];
	chara_limit.scmin = cval[6];
	chara_limit.scmax = cval[7];

	return TRUE;
}
#endif

#define HISTPREF_LIMIT 1024
static char *histpref_buf = NULL;

/*
 * Hook function for reading the histpref.prf file.
 */
void add_history_from_pref_line(cptr t)
{
	/* Do nothing if the buffer is not ready */
	if (!histpref_buf) return;

	my_strcat(histpref_buf, t, HISTPREF_LIMIT);
}


static bool do_cmd_histpref(void)
{
	char buf[80];
	errr err;
	int i, j, n;
	char *s, *t;
	char temp[64 * 4];
	char histbuf[HISTPREF_LIMIT];

#ifdef JP
	if (!get_check("生い立ち設定ファイルをロードしますか? ")) return FALSE;
#else
	if (!get_check("Load background history preference file? ")) return FALSE;
#endif

	/* Prepare the buffer */
	histbuf[0] = '\0';
	histpref_buf = histbuf;

#ifdef JP
	sprintf(buf, "histedit-%s.prf", player_base);
#else
	sprintf(buf, "histpref-%s.prf", player_base);
#endif
	err = process_histpref_file(buf);

	/* Process 'hist????.prf' if 'hist????-<name>.prf' doesn't exist */
	if (0 > err)
	{
#ifdef JP
		strcpy(buf, "histedit.prf");
#else
		strcpy(buf, "histpref.prf");
#endif
		err = process_histpref_file(buf);
	}

	if (err)
	{
#ifdef JP
		msg_print("生い立ち設定ファイルの読み込みに失敗しました。");
#else
		msg_print("Failed to load background history preference.");
#endif
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}
	else if (!histpref_buf[0])
	{
#ifdef JP
		msg_print("有効な生い立ち設定はこのファイルにありません。");
#else
		msg_print("There does not exist valid background history preference.");
#endif
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}

	/* Clear the previous history strings */
	for (i = 0; i < 4; i++) p_ptr->history[i][0] = '\0';

	/* Skip leading spaces */
	for (s = histpref_buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	/* Kill trailing spaces */
	while ((n > 0) && (s[n - 1] == ' ')) s[--n] = '\0';

	roff_to_buf(s, 60, temp, sizeof(temp));
	t = temp;
	for (i = 0; i < 4; i++)
	{
		if (t[0] == 0) break;
		else
		{
			strcpy(p_ptr->history[i], t);
			t += strlen(t) + 1;
		}
	}

	/* Fill the remaining spaces */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; p_ptr->history[i][j]; j++) /* loop */;

		for (; j < 59; j++) p_ptr->history[i][j] = ' ';
		p_ptr->history[i][59] = '\0';
	}

	/* Kill the buffer */
	histpref_buf = NULL;

	return TRUE;
}

/*
 *  Character background edit-mode
 */
/*:::生い立ち p_ptr->history[][]を編集する*/
static void edit_history(void)
{
	char old_history[4][60];
	int y = 0, x = 0;
	int i, j;

	/* Edit character background */
	for (i = 0; i < 4; i++)
	{
		sprintf(old_history[i], "%s", p_ptr->history[i]);
	}
	/* Turn 0 to space */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; p_ptr->history[i][j]; j++) /* loop */;

		for (; j < 59; j++) p_ptr->history[i][j] = ' ';
		p_ptr->history[i][59] = '\0';
	}
	display_player(1);
#ifdef JP
	c_put_str(TERM_L_GREEN, "(キャラクターの生い立ち - 編集モード)", 11, 20);
	put_str("[ カーソルキーで移動、Enterで終了、Ctrl-Aでファイル読み込み ]", 17, 10);
#else
	c_put_str(TERM_L_GREEN, "(Character Background - Edit Mode)", 11, 20);
	put_str("[ Cursor key for Move, Enter for End, Ctrl-A for Read pref ]", 17, 10);
#endif

	while (TRUE)
	{
		int skey;
		char c;

		for (i = 0; i < 4; i++)
		{
			put_str(p_ptr->history[i], i + 12, 10);
		}
#ifdef JP
		if (iskanji2(p_ptr->history[y], x))
			c_put_str(TERM_L_BLUE, format("%c%c", p_ptr->history[y][x],p_ptr->history[y][x+1]), y + 12, x + 10);
		else
#endif
		c_put_str(TERM_L_BLUE, format("%c", p_ptr->history[y][x]), y + 12, x + 10);

		/* Place cursor just after cost of current stat */
		Term_gotoxy(x + 10, y + 12);

		/* Get special key code */
		skey = inkey_special(TRUE);

		/* Get a character code */
		if (!(skey & SKEY_MASK)) c = (char)skey;
		else c = 0;

		if (skey == SKEY_UP || c == KTRL('p'))
		{
			y--;
			if (y < 0) y = 3;
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_DOWN || c == KTRL('n'))
		{
			y++;
			if (y > 3) y = 0;
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_RIGHT || c == KTRL('f'))
		{
#ifdef JP
			if (iskanji2(p_ptr->history[y], x)) x++;
#endif
			x++;
			if (x > 58)
			{
				x = 0;
				if (y < 3) y++;
			}
		}
		else if (skey == SKEY_LEFT || c == KTRL('b'))
		{
			x--;
			if (x < 0)
			{
				if (y)
				{
					y--;
					x = 58;
				}
				else x = 0;
			}

#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (c == '\r' || c == '\n')
		{
			Term_erase(0, 11, 255);
			Term_erase(0, 17, 255);
#ifdef JP
			put_str("(キャラクターの生い立ち - 編集済み)", 11, 20);
#else
			put_str("(Character Background - Edited)", 11, 20);
#endif
			break;
		}
		else if (c == ESCAPE)
		{
			clear_from(11);
#ifdef JP
			put_str("(キャラクターの生い立ち)", 11, 25);
#else
			put_str("(Character Background)", 11, 25);
#endif

			for (i = 0; i < 4; i++)
			{
				sprintf(p_ptr->history[i], "%s", old_history[i]);
				put_str(p_ptr->history[i], i + 12, 10);
			}
			break;
		}
		else if (c == KTRL('A'))
		{
			if (do_cmd_histpref())
			{
#ifdef JP
				if ((x > 0) && (iskanji2(p_ptr->history[y], x - 1))) x--;
#endif
			}
		}
		else if (c == '\010')
		{
			x--;
			if (x < 0)
			{
				if (y)
				{
					y--;
					x = 58;
				}
				else x = 0;
			}

			p_ptr->history[y][x] = ' ';
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x - 1)))
			{
				x--;
				p_ptr->history[y][x] = ' ';
			}
#endif
		}
#ifdef JP
		else if (iskanji(c) || isprint(c))
#else
		else if (isprint(c)) /* BUGFIX */
#endif
		{
#ifdef JP
			if (iskanji2(p_ptr->history[y], x))
			{
				p_ptr->history[y][x+1] = ' ';
			}

			if (iskanji(c))
			{
				if (x > 57)
				{
					x = 0;
					y++;
					if (y > 3) y = 0;
				}

				if (iskanji2(p_ptr->history[y], x+1))
				{
					p_ptr->history[y][x+2] = ' ';
				}

				p_ptr->history[y][x++] = c;

				c = inkey();
			}
#endif
			p_ptr->history[y][x++] = c;
			if (x > 58)
			{
				x = 0;
				y++;
				if (y > 3) y = 0;
			}
		}
	} /* while (TRUE) */

}

//出演作品　下のselect_unique_entry[]の添字に一致するように
#define ENTRY_REIMARI	0
#define ENTRY_KOUMA		1
#define ENTRY_YOUMU		2
#define ENTRY_EIYA		3
#define ENTRY_KAEI		4
#define ENTRY_HUUZIN	5
#define ENTRY_TIREI		6
#define ENTRY_SEIREN	7
#define ENTRY_SHINREI	8
#define ENTRY_KISHIN	9
#define ENTRY_KANZYU	10
#define ENTRY_TENKUU	11
#define ENTRY_KIKEI		12
#define ENTRY_KOURYUU	13
#define ENTRY_JUUOUEN	14
#define ENTRY_KINJYOU	15
#define ENTRY_TASOGARE	16
#define ENTRY_OTHER		17


#define ENTRY_MAX 18 //↑+1


const	cptr select_unique_entry[ENTRY_MAX] = {
		"a)定番自機二人",
		"b)紅魔郷",
		"c)妖々夢",
		"d)永夜抄",
		"e)花映塚",
		"f)風神録",
		"g)地霊殿",
		"h)星蓮船",
		"i)神霊廟",
		"j)輝針城",
		"k)紺珠伝",
		"l)天空璋",
		"m)鬼形獣",
		"n)虹龍洞",
		"o)獣王園",
		"p)錦上京",
		"q)黄昏弾幕アクション",
		"r)その他の作品",
	};

typedef struct unique_player_type unique_player_type;
struct unique_player_type
{
	bool flag_fix;  //まだ実装されてないとFALSE
	cptr name;		//名前
	int class_id;	//CLASS_???の値
	int race;		//RACE_???の値
	int entry;		//ENTRY_???の値　出演作品
	int sex;		//性別　たぶん今後も全員女だが一応
	cptr info;	//キャラメイク時に表示される説明文
};

#define UNIQUE_PLAYER_NUM 134
#define CLASS_DUMMY 255
#define RACE_DUMMY 255
static unique_player_type unique_player_table[UNIQUE_PLAYER_NUM] =
{
	{TRUE,"博麗　霊夢",CLASS_REIMU,RACE_HUMAN,ENTRY_REIMARI,SEX_FEMALE,
	"あなたは幻想郷の幾多の異変を解決に導いた博麗の巫女その人です。極めて高い潜在能力を持ちますが怠け者なのであまりやる気がありません。しかし神社への信仰(具体的にはお賽銭)が溜まればやる気が出るかもしれません。お賽銭は敵を倒したりクエストをこなすと参拝者が増えてだんだん溜まっていきます。また同じセーブファイルの以前の冒険で誰かが入れたお賽銭は全て引き継がれます。最終的には反則じみた強さになることでしょう。なおスコアは極めて低くなります。巫女がどこかにフラフラ飛んで行って異変の首謀者を叩きのめして帰ってくることは幻想郷において日常茶飯事であり誰も驚かないためです。"},
	{TRUE,"霧雨　魔理沙",CLASS_MARISA,RACE_HUMAN,ENTRY_REIMARI,SEX_FEMALE,
	"あなたは普通の魔法使いを自称していますが、あなたもまた幾多の異変を解決に導いた幻想郷の英雄です。身の軽さと手癖の悪さ、そして強力な攻撃魔法が身上です。あなたの独自の魔法は使用方法がやや特殊で、高級な魔法書を研究して製法を習得し、素材を集めて抽出した魔力から魔法を作り、作った魔法を持ち歩いて消費する形で使用します。素材の収集は地道な作業ですが、作られた魔法の威力は切り札と呼ぶにふさわしいものです。また同じセーブファイルの以前の冒険で誰かが霧雨魔法店に渡した素材アイテムも魔力として蓄積されています。魔法を作らなくても強力な特技を習得します。なお通常の魔法も習得可能で、第一領域と第二領域を自由に変更することができます。"},

	{TRUE,"ルーミア",CLASS_RUMIA,RACE_YOUKAI,ENTRY_KOUMA,SEX_FEMALE,
	"あなたは幻想郷の空をあてもなく漂う妖怪です。闇を操る能力があり、暗黒攻撃に関するスキルと耐性を持ちます。身体能力は通常の人間よりは高めですが妖怪としてはごく平凡です。武器の扱いはやや苦手で魔法は使えません。成長すると暗黒攻撃を全く受け付けなくなります。"},
	{TRUE,"大妖精",CLASS_DAIYOUSEI,RACE_FAIRY,ENTRY_KOUMA,SEX_FEMALE,
	"あなたは妖精の中で比較的強い力を持つ大妖精の一人です。固有の名前は持っていません。普段は霧の湖で遊んで暮らしています。あなたは霧に関係する能力を持ち、霧に隠れたり人を霧で惑わせることができるようです。魔法を一領域習得することも出来ますが基本的に荒事は全く不得手です。あなたの冒険はかなりの苦労を強いられることになるでしょう。" },
	{TRUE,"チルノ",CLASS_CIRNO,RACE_FAIRY,ENTRY_KOUMA,SEX_FEMALE,
	"あなたは普段は霧の湖で遊んで暮らしている氷の妖精です。妖精としては規格外といえるほど強い力を持ちますが、それはあくまでも妖精の中での話です。氷の妖精なので冷気攻撃に強く、成長すると冷気攻撃を全く受け付けなくなります。しかし火炎はあなたの大敵です。冷気関係の特技が豊富ですが、魔法書などはまったく読めず魔道具の扱いも苦手です。探索や解除など細かい作業も苦手です。"},
	{TRUE,"紅　美鈴",CLASS_MEIRIN,RACE_YOUKAI,ENTRY_KOUMA,SEX_FEMALE,
	"あなたは紅魔館で門番を務める妖怪で、武術の達人です。あなたの格闘時の通常攻撃は一行動当たりの攻撃回数が低めですが、消費行動力も通常よりやや短くなります。格闘攻撃を連続で行えば行うほど消費行動力がさらに減り実質的な攻撃力が上昇します。手堅く立ち回れば防御に長けますが、リスクを取ればかなりの攻撃力を発揮することが可能です。気を操る能力を持っており、感知や補助も多少こなします。高レベルになると破邪攻撃を弱点としなくなります。"},
	{TRUE,"パチュリー・ノーレッジ",CLASS_PATCHOULI,RACE_MAGICIAN,ENTRY_KOUMA,SEX_FEMALE,
	"あなたは生まれつきの魔法使いで、これまでの人生のほとんどを読書に費やして来ました。現在は紅魔館の大図書館に住んでいて外出はごく稀です。あなたは膨大な魔法の知識を持っており、ゲーム開始時点で魔術の4領域全てを記憶しています。レベルが足りていて魔法書を持っていれば魔法を行使可能です。さらに七曜を操る強力な魔法を持ち、時間経過による魔力の回復が極めて早いという特徴があります。しかし身体能力はかなり貧弱で重いものを持ち歩けず、まして接近戦などは全くの無謀です。喘息を患っており、喘息の調子が悪くなると魔法の失敗率が上がり、場合によっては巻物を読むことすら失敗するかもしれません。癒しの薬や生命の薬などで喘息の調子が良くなります。"},
	{TRUE,"十六夜　咲夜",CLASS_SAKUYA,RACE_HUMAN,ENTRY_KOUMA,SEX_FEMALE,
	"あなたは紅魔館を一手に切り盛りする人間のメイドで、趣味はナイフの蒐集です。あなたは時間を操るという人間離れした能力を持っており、ターン制限なしで時間を止めることができます。時間停止中に敵に直接攻撃することはできませんが、時間を止めたまま好きなだけ歩き回ったり休憩することができるため、感知や探索の概念が全く違った物になります。ほかにもナイフ投擲やテレポートなどに関する特技を豊富に習得します。あなたはメイドなので常にメイド服を着なければいけません。"},	
	{TRUE,"レミリア・スカーレット",CLASS_REMY,RACE_VAMPIRE,ENTRY_KOUMA,SEX_FEMALE,
	"あなたは紅魔館の主で、運命を操る力を持つという強力な吸血鬼です。白兵戦に極めて強く、多くの技能に優れ、多様な攻撃への耐性を持ち、魔法の習得も可能で、その上強力なアイテムがよく手に入ります。ただし吸血鬼なので破邪・閃光・水の攻撃に対しては脆弱です。強力な分スコアはかなり低くなります。"},
	{TRUE,"フランドール・スカーレット",CLASS_FLAN,RACE_VAMPIRE,ENTRY_KOUMA,SEX_FEMALE,
	"あなたは紅魔館の主の妹で、姉と同様強力な吸血鬼です。何百年も地下室にいて外になど興味を持ちませんでした。破壊の能力を持っており、ダンジョンのトラップやドアなどあらゆる障害を片端から壊して進みます。しかしあなたの破壊の力はしばしば壊すべきでないものまでうっかり壊します。様々な特技を持ちますがそれらの多くはやはり破壊的かつ不安定です。あなたの特異な精神はどのような恐ろしいものを見ても怯むことはありません。"},

	{TRUE,"レティ・ホワイトロック",CLASS_LETTY,RACE_YOUKAI,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは寒気を操る妖怪です。暑い間はどこかに隠れ住んでいて冬になると外に出てきます。冷気・睡眠攻撃が得意で、うまくいけば敵に行動させずに倒すことも可能です。生物に対しては有利に戦えますがアンデッドやゴーレムが相手だとほとんど無力です。冷気に強い耐性を持つ一方で炎はあなたの大敵です。冒険の序盤は楽にこなせますが終盤はかなりの苦労を強いられることでしょう。あなたは自ら気候を変えるような力は持っていませんが、吹雪の中では最大の力を発揮することができます。"},
	{TRUE,"橙",CLASS_CHEN,RACE_WARBEAST,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは大妖怪八雲紫の式である八雲藍のそのまた式です。苗字はまだありません。敵の不意を衝くのがうまく、離れた敵に一瞬で駆け寄ったり一撃を加えて離脱するなど素早い行動を得意とします。しかし水攻撃を受けたり浮遊せずに水に入ると式が剥がれて弱体化してしまいます。式の憑いていないあなたはごく平凡な化け猫です。式が剥がれているときに探索拠点に行くと主に式を憑け直してもらえます。武器や魔道具の扱いはやや苦手です。"},
	{TRUE,"アリス・マーガトロイド",CLASS_ALICE,RACE_MAGICIAN,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは人形遣いです。完全に自律して動く人形を作ることを目指し日々研究を続けています。あなたの戦闘スタイルは独特で、自らは武器を持たずに魔法の糸で操る人形たちを武装させて戦います。人形を操る傍らで魔法を一領域行使可能です。人形の数はレベルアップにより増え、人形の攻撃力は知能、器用さ、人形の種類、そして持たせた武器で決まります。人形の持った武器にかかった耐性や能力上昇効果などのいくつかは発揮されません。人形による攻撃は強力ですがあなた自身はかなり打たれ弱いため良質な防具の確保が重要です。しかし人形を操る邪魔になるのでグローブを装備することはできません。あなたは精密な魔法を常に使い続けているため劣化攻撃が弱点です。騎乗は全くできません。"},
	{TRUE,"リリーホワイト",CLASS_LILY_WHITE,RACE_FAIRY,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは春の到来を知らせる妖精で、春告精とも呼ばれます。今は春が来て元気いっぱいのため普通の妖精より明らかに強い力を持ちます。しかし30日経つと春の終わりが近づき、やる気をなくしてしまいます。それまでに*勝利*しないとゲームオーバーです。あなたの戦闘能力は高いとは言えず、特筆に値するほどの長所や特技も持っていません。あなたの冒険は過酷なものとなるでしょう。"},
	{TRUE,"ルナサ・プリズムリバー",CLASS_LUNASA,RACE_SPECTRE,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは騒霊の姉妹の長女です。いつも冷静沈着で真面目です。魔法を習得することができ、肉弾戦はあまり好みではありません。あなたの奏でる音には聞く者の気分を沈める力があります。あなたたちは自宅である森の廃洋館に行って他の姉妹と冒険を交代することができます。経験値、基礎パラメータ、所持品などは姉妹で共有されます。あなたは霊体のためダンジョンで壁を抜けることができます。冒険において非常に有利ですがスコアが大幅に下がり、また破邪攻撃はあなたの大敵です。なお、一度も他の姉妹と交代せずに勝利したらスコアが上がります。"},
	{TRUE,"メルラン・プリズムリバー",CLASS_MERLIN,RACE_SPECTRE,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは騒霊の姉妹の次女です。いつも明るく朗らかで、時には意思の疎通が困難なほどハイテンションです。あなたの奏でる音には聞く者の気分を高揚させる力があります。あなたたちは自宅である森の廃洋館に行って他の姉妹と冒険を交代することができます。経験値、基礎パラメータ、所持品などは姉妹で共有されます。あなたは霊体のためダンジョンで壁を抜けることができます。冒険において非常に有利ですがスコアが大幅に下がり、また破邪攻撃はあなたの大敵です。なお、一度も他の姉妹と交代せずに勝利したらスコアが上がります。"},
	{TRUE,"リリカ・プリズムリバー",CLASS_LYRICA,RACE_SPECTRE,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは騒霊の姉妹の三女です。お調子者ですが計算高く目端が利き、大抵のことを器用にこなします。あなたの奏でる音は通常の世界には存在しない音で、様々な不思議な効果を起こします。あなたたちは自宅である森の廃洋館に行って他の姉妹と冒険を交代することができます。経験値、基礎パラメータ、所持品などは姉妹で共有されます。あなたは霊体のためダンジョンで壁を抜けることができます。冒険において非常に有利ですがスコアが大幅に下がり、また破邪攻撃はあなたの大敵です。なお、一度も他の姉妹と交代せずに勝利したらスコアが上がります。"},



	{TRUE,"魂魄　妖夢",CLASS_YOUMU,RACE_HALF_GHOST,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは半人半霊の剣士です。冥界で庭師兼護衛として主に仕えています。小柄ながらかなり鍛えられており、二刀流の達人です。「楼観剣」「白楼剣」という由緒ある刀を最初から所持しています。特異な体質と気質を活かした独特の剣技で戦います。また武芸の秘伝書を読むことで強力な技の習得が可能です。"},
	{TRUE,"西行寺　幽々子",CLASS_YUYUKO,RACE_SPECTRE,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは白玉楼の主を務める強力な亡霊です。茫洋とした見た目に反し高い近接戦闘能力を持ちます。また死霊領域の魔法を得意とし、さらに死の力を操ったりダンジョンの敵を消し去る特技を持ちます。様々な攻撃に対する耐性をもち、地獄攻撃を完全に無効化する一方で閃光を弱点とせず、さらに壁を抜けて移動することができます。ダンジョン探索において完璧に近い総合力の持ち主ですが、唯一の弱点としてすぐにお腹が空きます。食料は大量に持ち歩きましょう。なおあなたはお菓子を食べてMPを回復することができます。"},
	{TRUE,"八雲　藍",CLASS_RAN,RACE_YOUKO,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは大妖八雲紫の式です。主から鉄獄最深層への刺客を命じられました。あなた自身も大妖怪として知られる九尾の狐であり、加えて主から強力な式を付与されているためあらゆる面で極めて高い能力を持ちます。しかし命令完遂までの期限は僅か一週間です。脇目も振らずにダンジョンへ潜り続ける必要があるでしょう。なお、鉄獄の位置は最初から知らされており帰還の魔法などで行くことができます。"},
	{TRUE,"八雲　紫",CLASS_YUKARI,RACE_DAIYOUKAI,ENTRY_YOUMU,SEX_FEMALE,
	"あなたは幻想郷最古参の大妖怪で、幻想郷を維持管理する役目を負っています。「境界」を操作する能力を持ち、強力かつ奇怪な特技を習得します。基礎的な能力値も高く特に知能の高さは全職業中トップクラスです。ただしスコア倍率は極めて低くなり、また成長もかなり遅めです。"},

	{TRUE,"リグル・ナイトバグ",CLASS_WRIGGLE,RACE_YOUKAI,ENTRY_EIYA,SEX_FEMALE,
	"あなたは蛍の妖怪です。蟲を操る能力を持っており、蟲たちを使役して戦わせたり周囲を感知するなどの特技を習得します。あなたは光源がなくてもダンジョンを照らすことができ、レベルが上がると閃光への耐性を得ます。ただし寒さはあなたの大敵です。あなた自身の戦闘能力はそれほど高くはないため、中盤以降は苦戦を強いられるかもしれません。"},
	{TRUE,"ミスティア・ローレライ",CLASS_MYSTIA,RACE_YOUKAI,ENTRY_EIYA,SEX_FEMALE,
	"あなたは歌声で人を惑わせる夜雀の妖怪です。魔力を持った歌を歌うことで聴く者を鳥目にしたり混乱させることができます。また鳥系のモンスターを支配したり召喚する特技を持ちます。戦闘能力は標準的でやや敏捷性に優れます。"},
	{TRUE,"上白沢　慧音",CLASS_KEINE,RACE_WARBEAST,ENTRY_EIYA,SEX_FEMALE,
	"あなたは半獣ですが、人間からの信頼が厚く人里の寺子屋で教師をして暮らしています。戦闘においては白兵戦、魔法など一通りのことをそれなりにこなします。歴史を食べる能力を持ち、様々な存在を隠蔽する特技を習得します。獣人には珍しく極めて高い知性を持ちますが、かなりの堅物で石頭です。普段は獣らしいところはほとんどありませんが、満月を見ると獣側である白沢(ハクタク)の力が色濃く現れ飛躍的にパワーアップします。白沢になる時には大きな角が生えるため、頭の装備品には気をつけてください。"},
	{TRUE,"因幡　てゐ",CLASS_TEWI,RACE_WARBEAST,ENTRY_EIYA,SEX_FEMALE,
	"あなたはかなり古株の妖怪兎で、永遠亭で兎達を率いています。狡知に長けており罠や一撃離脱を駆使して戦います。また、どこで稼いだのかゲームスタート時に大量の資金を持っています。しかし基礎的な戦闘能力はお世辞にも高いとはいえず、冒険の終盤では苦戦することでしょう。"},
	{TRUE,"鈴仙・優曇華院・イナバ",CLASS_UDONGE,RACE_GYOKUTO,ENTRY_EIYA,SEX_FEMALE,
	"あなたは月から来た妖怪兎です。のんびり屋の多い玉兎の中では卓越した戦闘センスを持っていました。波長を操るという極めて応用性に富んだ能力を持ち、攻撃防御感知と多彩な特技を習得します。地上での暮らしが長いため玉兎の破邪攻撃耐性が失われています。"},
	{TRUE,"八意　永琳",CLASS_EIRIN,RACE_HOURAI,ENTRY_EIYA,SEX_FEMALE,
	"あなたはかつて月の賢者と呼ばれた高貴な存在でしたが、今は主の輝夜と共に地上人として暮らしています。あなたは天才的な頭脳を持ち、レベルが足りてて魔法書を持っていれば全ての魔法を行使可能です。さらにあらゆる調剤のレシピを記憶しており、弓の扱いが得意で、白兵戦もこなし、その上一切の毒物を受け付けません。あなたは輝夜の言いつけにより共に鉄獄に物見遊山の旅に出かけました。輝夜の能力はフロアを移動するたびにあなたのレベルに合わせて変動します。主を守りながら冒険を完遂しましょう。あなたと輝夜のどちらか片方でも倒れるとゲームオーバーです。"},
	{TRUE,"蓬莱山　輝夜",CLASS_KAGUYA,RACE_HOURAI,ENTRY_EIYA,SEX_FEMALE,
	"あなたはお伽噺に語られるかぐや姫その人です。幻想郷で永遠亭の主として長い間ひっそりと暮らしていましたが、最近あまりに暇なため、遠い昔に探し求めた五つの神宝を今度は自分で探し出そうと一念発起しました。『モルゴス』『秩序のユニコーン』『金無垢の指輪』『オメガ』『アザトート』という強敵がそれらを保有しているようです。頑張って倒して手に入れましょう。もし同じセーブデータの以前の冒険者がこれらの神宝をあなたに届けていた場合、あなたはそれをゲーム開始時から所有しています。ただし開始時に一つ所有しているごとにスコアが半分になります。あなたは不死身の蓬莱人ですが、一度倒れたら冒険に飽きてしまうためゲームオーバーです。"},
	{TRUE,"藤原　妹紅",CLASS_MOKOU,RACE_HOURAI,ENTRY_EIYA,SEX_FEMALE,
	"あなたは千年以上前に蓬莱の薬を飲んで不老不死となった人間です。HPが尽きてもその場で復活するという掟破りの能力を持っています。復活するときにはMPを消費し、HPとMPがどちらも無くなるとゲームオーバーです。戦闘において多少以上に無茶が利きますが、MPの残量には気を配りましょう。不老不死であることを除けばあなたの身体能力はそれほど高くありませんが、数百年にわたり戦いの経験を積んでいるため様々な技能に優れます。魔法を一領域習得可能ですが、あなたの体は一切の変容を受け付けないため変身魔法の使用はできません。あなたは千数百年の生にやや飽いており、レベルアップには多くの経験値が必要です。また復活のたびにスコアが大幅に減少します。"},

	{TRUE,"風見　幽香",CLASS_YUUKA,RACE_DAIYOUKAI,ENTRY_KAEI,SEX_FEMALE,
	"あなたは強力な妖怪です。花を咲かせる能力を持ちますが戦闘で能力を使用することはほとんどなく、純粋に高い妖力と身体能力で敵を圧倒します。さらに最初から強力な武器を持っているため、戦いで後れを取ることはほとんどないでしょう。ただし成長は極めて遅く、スコアも大幅に下がります。"},
	{TRUE,"メディスン・メランコリー",CLASS_MEDICINE,RACE_ANDROID,ENTRY_KAEI,SEX_FEMALE,
	"あなたは無名の丘に捨てられていた人形です。まだ自我を得てからあまり経っておらず、色々な所に行って見聞を広めようとしています。あなたは様々な毒を操る力を持っており、格闘時には全ての攻撃が強力な毒属性を持ちます。あなたの体は鈴蘭の毒で満たされており、毒を吸収して体力と満腹度を回復させることができますが、通常の妖怪人形と異なり魔道具を食事にすることができません。あなたの純粋な身体能力は平凡です。普通の生物に対しては有利に戦えますが毒の効かない敵には苦戦することでしょう。経験値を得ることでなく強力な装備をすることによってレベルアップします。"},
	{TRUE,"小野塚　小町",CLASS_KOMACHI,RACE_DEATH,ENTRY_KAEI,SEX_FEMALE,
	"あなたは三途の川で船頭をして暮らしている死神です。しばしば仕事を放り出して様々なことに首を突っ込みます。距離を操るという便利な能力を持ち、テレポート系の特技が充実しています。幽霊を使役したり生者を一撃で倒すなど死神らしい技も持っています。上司である四季映姫には頭が上がりません。"},
	{TRUE,"四季映姫・ヤマザナドゥ",CLASS_EIKI,RACE_DEITY,ENTRY_KAEI,SEX_FEMALE,
	"あなたは幻想郷担当の閻魔様です。閻魔になる前はお地蔵様でした。白黒はっきりつけるという能力を持ち、この能力によりゲーム中において全てのキャラクターの行動順に乱数が関わらなくなります。またあなたは人を裁くことに特化した孤高かつ異質な精神を持っており、あらゆる精神攻撃を無効化します。あなたの武器は「悔悟の棒」と呼ばれる板です。敵のレベルが高いほど威力が上がり、何人たりともその一撃から逃れることはできません。他にも様々な強力な能力や特性を持ちますが、成長は遅くスコアも大幅に下がります。"},

	{TRUE,"秋　静葉",CLASS_SHIZUHA,RACE_STRAYGOD,ENTRY_HUUZIN,SEX_FEMALE,
	"あなたは紅葉と落葉の神様です。しかしあなたを熱心に信仰する者はほとんどいません。冬が嫌いで、冷気攻撃を受けると少しダメージが大きくなります。華奢な外見の割にあなたの身体能力は高く、主に格闘で戦います。しかし耐久力はかなり低めで、特筆するほどの特技も持ちません。あなたの総合的な戦闘能力はその辺の妖怪以下ですが、冒険を続け様々な経験を積んでいるといずれは新たな信仰を獲得して大幅にパワーアップするかもしれません。"},
	{TRUE,"秋　穣子",CLASS_MINORIKO,RACE_STRAYGOD,ENTRY_HUUZIN,SEX_FEMALE,
	"あなたは実りの秋を司る神様です。人々に好かれる神様ですが、豊穣神には他に有名な神様が多いのであなた自身はあまり信仰を得られていません。庶民的な気取らない神様で荒事は苦手です。武器の扱いも不得手ですが収穫を象徴する大鎌の扱いはそれなりに巧みです。自然魔法を習得できますが、自然魔法はやや攻撃力に欠けるのである程度の肉弾戦を覚悟する必要があります。またあなたは冬が苦手なため冷気攻撃によってやや大きなダメージを受けます。総合的な戦闘能力は低めですが、冒険を通じて新たな信仰を得ると全く別の神格に変化するかもしれません。"},
	{TRUE,"鍵山　雛",CLASS_HINA,RACE_DEITY,ENTRY_HUUZIN,SEX_FEMALE,
	"あなたは厄神です。神と呼ばれてはいますが実態は妖怪に近いようで、破邪攻撃が弱点です。厄を吸収してその身に溜め込む力を持っています。厄は呪われたアイテムから吸い取ったり敵から邪悪な攻撃を受けることで溜まり、特技で消費したり敵に押し付けることで減ります。あなたは厄が溜まれば溜まるほど強くなりますが、溜まり過ぎると極めて危険な事になるので厄の量には常に気を配って下さい。あなたは人から避けられており店での売買に苦労しますが、人里の雛人形販売所で幾許かの資金を得ることができます。またあなたはマイナス効果をもつ装備品を身につけて戦い続けることでその装備品のマイナス効果を打ち消したり反転させることができます。しかしそれは極めて困難です。"},
	{TRUE,"河城　にとり",CLASS_NITORI,RACE_KAPPA,ENTRY_HUUZIN,SEX_FEMALE,
	"あなたは河童のエンジニアで、通常のエンジニアと同様に大金を使って機械の開発が可能です。開発した機械を格納するリュックの容量は少なめですが、その代わりに特技として独自の機械を使用可能になります。独自の機械は開発をしなくても特定レベルで使用可能になり、エネルギーは水地形上で充填可能です。通常のエンジニアの機械よりやや攻撃力に欠けますがコストの面では有利です。またリュックの中には様々な近接戦闘用ギミックが仕込まれており通常のエンジニアに比べ格闘能力が高めです。水を操る能力を持っておりいくつかの水攻撃の特技を習得します。"},
	{TRUE,"犬走　椛",CLASS_MOMIZI,RACE_HAKUROU_TENGU,ENTRY_HUUZIN,SEX_FEMALE,
	"あなたは妖怪の山で暮らす下っ端の哨戒天狗です。千里眼の能力を持ち、感知や探索に関しては飛び抜けて優れています。光源がなくても暗いダンジョンを見通し、レベルが上昇すると盲目に対する耐性を獲得し、さらに周辺のあらゆる敵を常時感知するようになります。ただし戦闘能力はごく平凡です。なお、あなたは頭襟以外の頭防具を装備できません。"},
	{TRUE,"射命丸　文",CLASS_AYA,RACE_KARASU_TENGU,ENTRY_HUUZIN,SEX_FEMALE,
	"あなたは幻想郷の少女たちの新聞を書くことに情熱を燃やす鴉天狗です。見た目に反して鴉天狗の中でもかなり古株です。全キャラクター最高の加速と敏捷性を持ち、広域マップ上では移動速度がさらに増加する上一切の襲撃を受けません。接近戦に強く、神秘か自然の魔法を一領域習得可能で、さらに便利な特技を多く持つ万能型です。ただし成長はかなり遅めでスコアも大幅に低下します。"},
	{TRUE,"東風谷　早苗",CLASS_SANAE,RACE_DEMIGOD,ENTRY_HUUZIN,SEX_FEMALE,
	"あなたは風の神に仕える神職で、風祝(かぜはふり/かぜほうり)と呼ばれます。信仰が失われつつある外の世界から神様と共に幻想郷へと移り住んできました。あなた自身も神の血を引いており、代々伝わる秘術で強力な奇跡を起こします。しかしそのためには数ターンから場合によっては数十ターンの長い詠唱が必要です。敵の動きを先読みして詠唱しながら待ち伏せるなど戦略的な立ち回りが要求されることでしょう。あなたの身体能力は普通の人間と変わらず、肉弾戦はそれほど得意ではありません。"},
	{TRUE,"八坂　神奈子",CLASS_KANAKO,RACE_DEITY,ENTRY_HUUZIN,SEX_FEMALE,
	"あなたは信仰の失われつつある外の世界から神社ごと幻想郷へ移住してきた神様で、新天地で新たな信仰を得ようと精力的に活動しています。あなたの序盤の戦闘能力は平凡ですが、名のある敵を打ち倒すたびに信仰が集まってあなたはどんどん強くなります。特に特定の宗教の代表的な存在を倒すと大幅に信仰が集まり、新たな耐性を得ることがあります。最終的には大変な強さになりますが、成長は遅くスコアも大幅に低下します。"},
	{TRUE,"洩矢　諏訪子",CLASS_SUWAKO,RACE_DEITY,ENTRY_HUUZIN,SEX_FEMALE,
	"あなたはかつて土着の神々を束ねる強大な神格でしたが、現在は神社の経営を神奈子に任せて気楽な神様生活を送っています。配下の祟り神の力を借りて大地を操る特技をもち、妖術を全領域行使でき、さらに白兵戦もそれなりにこなします。今は昔の力を失っていてMPが低く魔法や特技の使用に苦労しますが、人間系の敵を倒し続けて皆から恐れられるとMP最大値が上昇します。総合的には極めて強力ですが成長はかなり遅くスコアも大幅に低下します。"},

	{TRUE,"キスメ",CLASS_KISUME,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"あなたは釣瓶落としの妖怪で、誰かの頭上に落下することがあなたの生き甲斐です。いつも桶の中に入っており、胴には桶以外の物を装備できず、そのうえ足装備も不可能です。あなたの格闘攻撃は上空からの落下です。攻撃回数は常に1ですが威力は高く、入っている桶が丈夫なものだとさらに威力が跳ね上がります。高く飛び上がることにより落下攻撃の威力を大幅に増加させる特技を持ちます。"},
	{TRUE,"黒谷　ヤマメ",CLASS_YAMAME,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"あなたは土蜘蛛の妖怪です。病気を操る力を持ち、毒耐性を持たない敵に攻撃すると大きなダメージを与えることができます。また糸を使って敵の動きを妨害したり自分が高速で移動することができます。あなたは力持ちで穴掘りが得意ですが、武器の扱いなどはあまり得意ではありません。"},
	{TRUE,"水橋　パルスィ",CLASS_PARSEE,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"あなたは嫉妬深い橋姫です。嫉妬をしたりされたりすることがあなたの力になります。敵のレベルが自分より高いと攻撃力が上昇し、特にその敵が女性だと攻撃力が強烈に跳ね上がります。また体力が減っているほど攻撃力や速度が上昇します。ほかにもいくつか特技を持ちますが、ほとんどが自分の体力が減っていたり敵が強いほど強力になるようなハイリスクなものです。あなたの基礎的な戦闘能力は低いため強敵を相手にするときは危険を冒す必要があるでしょう。"},
	{TRUE,"星熊　勇儀",CLASS_YUGI,RACE_ONI,ENTRY_TIREI,SEX_FEMALE,
	"あなたは鬼の四天王の一人に数えられる強大な鬼です。その比類なき豪腕で不条理なまでの破壊を引き起こします。あなたは最高の腕力を持ちますが武器や格闘などの技能の鍛錬にはあまり興味がなく、純粋な力で敵をねじ伏せることを好みます。あなたは常に片手に盃を持っていて、盃の不思議な力でお酒を美味しく飲むことができます。"},
	{TRUE,"古明地　さとり",CLASS_SATORI,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"あなたは地霊殿の主の覚(さとり)妖怪です。嫌われ者が集まる地底においても一際嫌われており、普段は外に出ることすらなくペット達に囲まれて暮らしています。あなたは胸の第三の目で他者の心を読み、他者のトラウマを刺激して精神を攻撃し、さらには読んだ心象を現実に再現する特技を持ちます。ただしこれらの特技は精神を持たない相手に対しては全く無力です。あなたの身体能力は妖怪としてはかなり低いので戦う相手と戦略は慎重に選びましょう。レベルが上がると壁の向こうの敵を感知し、物に残留する思念を読み取り、さらに混乱・恐怖・狂気に対する耐性を得ます。"},
	{TRUE,"火焔猫　燐",CLASS_ORIN,RACE_WARBEAST,ENTRY_TIREI,SEX_FEMALE,
	"あなたは火車と呼ばれる化猫です。普段は地霊殿の旧灼熱地獄で働いています。葬式に侵入して死体を持ち去るのが大好きで、人里ではかなり嫌われています。いつも死体を入れるための猫車を押しており、この中にはアイテムを収納できます。収納したアイテムは敵の攻撃で破壊されませんが一度出さないと使えません。隠密や探索に優れ、魔法を習得し、さらに猫車で敵を轢いて攻撃することができます。火炎と地獄の攻撃に耐性を持ち、成長すると火炎の二重耐性と狂気耐性を獲得します。ただし破邪攻撃に対しては極めて脆弱です。"},
	{TRUE,"霊烏路　空",CLASS_UTSUHO,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"あなたは地霊殿でペットとして暮らす平凡な地獄烏でしたが、ある日八咫烏をその身に取り込んで飛躍的にパワーアップし、あろうことか核融合を操る力を手にしました。今やあなたの体は無尽蔵のエネルギーの塊です。強力な遠隔攻撃を撃ち放題ですが、ほとんどの攻撃が火炎や閃光属性のため耐性を持つ敵が相手だとやや手こずるかもしれません。あなたの魔道具や罠解除などの技能は押しなべて低く、ぶっ放して焼き払う以外のことはほとんどできません。また核融合の制御のためには片腕を開けておかなければならず、グローブとブーツはそれぞれ片方しか装備できないため性能が低下します。"},
	{TRUE,"古明地　こいし",CLASS_KOISHI,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"あなたは瞳と心を閉ざした覚の妖怪です。ほとんど明瞭な意識を持たずに幻想郷のあちこちを徘徊しています。読心の瞳を閉じた代わりに無意識を操る能力を得ており、敵はしばしば目の前にいるあなたを見失います。敵の不意をついて強烈な一撃を見舞うことも容易でしょう。ただし通常の精神を持たない相手に対してはこの能力は効果が低下します。あなたの精神はいかなるものからも影響を受けません。"},

	{TRUE,"ナズーリン",CLASS_NAZRIN,RACE_WARBEAST,ENTRY_SEIREN,SEX_FEMALE,
	"あなたは毘沙門天の使いの妖怪鼠で、現在は命蓮寺に協力しつつ無縁塚の近くで暮らしています。ダウジングの名手で宝探しはお手の物です。さらに鼠召喚や一撃離脱などの小技も持ち、隠密行動や魔道具使用も得意です。またあなたは毘沙門天の使いであるため妖怪であるにもかかわらず破邪攻撃を弱点としません。しかしあなたの体格はかなり貧弱で、戦闘能力はお世辞にも高いとは言えません。かなり慎重な立ち回りが要求されますがそれでも危険は大きいでしょう。"},
	{TRUE,"多々良　小傘",CLASS_KOGASA,RACE_TSUKUMO,ENTRY_SEIREN,SEX_FEMALE,
	"あなたは唐傘お化けと呼ばれる傘の付喪神です。手に持った傘があなたの本体で、手放すことはできません。あなたはこの傘を振り回して戦うことになります。傘なので水には強いのですが酸と火炎は苦手です。付喪神の種族特性を合わせると、初期では酸と火炎と破邪と劣化が弱点ということになります。レベルが上がると傘もパワーアップし、いくつかは改善されるかもしれません。通常の付喪神と違い魔道具で食事を摂ることができず、誰かを驚かせることで活力を得ます。またあなたは鍛冶が得意で、武具の目利きをしたり金属製の武具を強化することができます。"},
	{TRUE,"雲居　一輪",CLASS_ICHIRIN,RACE_YOUKAI,ENTRY_SEIREN,SEX_FEMALE,
	"あなたは入道使いという珍しい妖怪で、雲のような姿の「雲山」と呼ばれる入道といつも一緒にいます。格闘攻撃をするとき、あなたの代わりに雲山が攻撃を行います。雲山の巨大な拳にはかなりの威力が期待でき、攻撃の威力はほとんどあなたの腕力に関係なく決まります。装備が重すぎると攻撃に差し障りが出るのは通常の格闘と同じです。雲山は離れた場所に攻撃したりレーザーを放つなど便利な特技を持ち、さらにピンチのときにはしばしばあなたを守ってくれます。しかしあなたは雲山との連携のため常に両手を開けておく必要があり、手が塞がると攻撃力が大幅に低下します。"},
	{TRUE,"村紗　水蜜",CLASS_MURASA,RACE_YOUKAI,ENTRY_SEIREN,SEX_FEMALE,
	"あなたは舟幽霊で、趣味は人を溺れさせることです。水のある所どこにでも出現します。幽霊とはいっても肉体を持つ念縛霊で、比較的妖怪に近い体をしています。水に関する多彩な特技をもち、敵の水攻撃を完全に無効化することができます。また水の中では高速で移動でき、パラメータが強化され、さらに素手攻撃により敵を一撃で倒すことがあります。しかし水棲生物が相手だとほとんどの攻撃手段は無力です。幽霊の一種なので破邪攻撃は大の苦手です。"},
	{TRUE,"寅丸　星",CLASS_SHOU, RACE_WARBEAST ,ENTRY_SEIREN,SEX_FEMALE,
	"あなたは人々の空想から生まれた妖獣でしたが、色々あって今は命蓮寺で毘沙門天の代理を務めています。半分神格化しているため妖怪であるにもかかわらず破邪攻撃に耐性を持ちます。身体能力が高く、獣の鋭敏な感覚を持ち、魔道具などの技能もそこそこ優秀です。財宝が集まる能力がありあまり資金に苦労しません。さらに最初から毘沙門天の宝塔を所持しておりこれを使って強力な遠隔攻撃が可能です。全体的に隙のない能力の高さですが、勇ましい見た目に反して武術の心得は全くありません。頑張って鍛えましょう。宝塔を手放すと多くの特技が使えなくなるため、うっかり無くさないように気をつけて下さい。"},
	{TRUE,"聖　白蓮",CLASS_BYAKUREN,RACE_MAGICIAN,ENTRY_SEIREN,SEX_FEMALE,
	"あなたは千年にわたる封印から解き放たれた強力な魔法使いです。毘沙門天に帰依する徳の高い僧侶でもあり、現在は命蓮寺の実質的な指導者として修行と布教をしながら暮らしています。あなたの特技は強力な身体強化魔法による肉弾戦で、立ち塞がる敵を次々と殴り倒すことができるでしょう。ただし強化の魔法が切れると打たれ弱いので油断は禁物です。総合的に極めて強力ですが、スコアは大幅に低下します。"},
	{TRUE,"封獣　ぬえ",CLASS_NUE,RACE_DAIYOUKAI,ENTRY_SEIREN,SEX_FEMALE,
	"あなたはかつて平安京を恐怖に陥れたと噂される大妖怪です。物事に対する認識を惑わせる能力を持ち、敵の精神へ攻撃を仕掛けるのが得意です。光源がなくてもダンジョンを見通すことができ、暗い所では敵から発見されづらくなります。またあなたは自分の正体を隠して怪物に変身しています。変身は強力ですが敵に正体を見破られてしまうとその敵がフロアからいなくなるまで変身が解除されます。明るい場所にいたり賢い敵であったりあなたの苦手な属性の攻撃をされたときは正体を見破られやすいようです。総合的に強いぶん成長は遅くスコアもかなり低めです。"},

	{TRUE,"幽谷　響子",CLASS_KYOUKO,RACE_YOUKAI,ENTRY_SHINREI,SEX_FEMALE,
	"あなたは幽谷響（ヤマビコ）の妖怪で、犬のような耳と尻尾を持っています。朝早くから命蓮寺で真面目に修行に励む一方で、夜はパンクロックに傾倒する意外な一面も持ちます。大声とオウム返しが特技で、轟音で敵を朦朧とさせたり敵の唱えた魔法を真似することができます。レベルが上がると轟音攻撃を完全に無効化し、敵のボルト系魔法を反射することができます。ただしあなたの身体能力はそれほど高くなく、基本的に荒事は苦手です。大声で戦うため隠密行動も大の苦手です。"},
	{TRUE,"宮古　芳香",CLASS_YOSHIKA,RACE_ZOMBIE,ENTRY_SHINREI,SEX_FEMALE,
	"あなたは邪仙霍青娥の愛用のキョンシーです。お肌のケアは万全ですが脳は完全に腐っています。主に毒爪のひっかき攻撃で戦います。底なしの体力を持ち、体力の自然回復が極めて早く、さらに周囲の小神霊を食べて体力を回復することができます。しかし解除や魔道具など頭や手先を使う作業はおしなべて苦手です。かなりの勢いで腹が減るので食料は多めに持ちましょう。"},
	{TRUE,"霍　青娥",CLASS_SEIGA,RACE_SENNIN,ENTRY_SHINREI,SEX_FEMALE,
	"あなたは道を外れた仙人です。基本的に自分の欲望と好奇心を満たす事しか考えていません。特殊な鑿で壁に穴を開ける特技を持ちます。レベルが上がると移動時に壁を掘り抜き、さらにレベルが上がると壁を掘らずにすり抜けるようになります。魔法を二領域習得可能で、死霊術などの禍々しい術に特に高い適性を持ちます。長い時を生きた仙人のため、しばしば地獄からの刺客に狙われます。"},
	{TRUE,"蘇我　屠自古",CLASS_TOZIKO,RACE_SPECTRE,ENTRY_SHINREI,SEX_FEMALE,
	"あなたは豊聡耳神子に従者として仕える亡霊です。かつては強力な怨霊でしたが、今は恨みもほとんど消え気ままな亡霊生活を送っています。霊体のため壁を抜けることができダンジョン探索に有利です。電撃攻撃が得意で、武器の扱いは弓を除き不得手です。下半身の具現化が不完全なため靴を履くことができません。"},
	{TRUE,"物部　布都",CLASS_FUTO,RACE_SENNIN,ENTRY_SHINREI,SEX_FEMALE,
	"あなたは千四百年の眠りから目覚めた尸解仙で、豊聡耳神子の元で道士として修行の日々を送っています。仙人の割に魔法書の扱いはあまり得意でなく、代わりに物部の秘術と道教の仙術を合わせた独特の技で戦います。この技はやや癖がありますが入念に準備すれば威力は絶大です。あまり乱発すると皿代が嵩むため序盤はうまく遣り繰りしましょう。あなたの身体能力はそれほど高いとはいえず、弓以外の武器の扱いは苦手です。またあなたは秘術により本来の寿命を誤魔化しているため、ダンジョン内で地獄からの刺客に襲われることがあります。"},
	{TRUE,"豊聡耳　神子",CLASS_MIKO,RACE_SENNIN,ENTRY_SHINREI,SEX_FEMALE,
	"あなたは千四百年の時を経て復活を遂げ神格化した聖人です。「人間を超えるべくして超えた」と言われるほどの圧倒的な才能を持ちます。仙術の四領域の魔法を全て習得しており、レベルが足りていて魔法書を持っていれば学習をしなくても魔法を使えます。さらにあらゆる技能に優れ、強力な特技を使い、優秀な耐性をもち、白兵戦も高水準にこなします。完全無欠に近い強さですが、スコアは極めて低くなります。"},
	{TRUE,"二ッ岩　マミゾウ",CLASS_MAMIZOU,RACE_BAKEDANUKI,ENTRY_SHINREI,SEX_FEMALE,
	"あなたは化け狸の親分で、妖怪の身でありながらつい最近まで外界に住んでいました。幻想郷でも早々に社会に溶け込み、様々な企みごとに一枚噛んでいます。あなたは化け力(ばけぢから)が自慢で、敵や自分自身を好き放題に変身させて敵を翻弄します。また、狸の配下を呼び出して敵を攻撃することもできます。変身系の特技は知能と賢さ、配下を使う特技は魅力によって効果が大きく変わります。"},

	{TRUE,"わかさぎ姫",CLASS_WAKASAGI,RACE_NINGYO,ENTRY_KISHIN,SEX_FEMALE,
	"あなたは霧の湖で歌を歌ったり石を拾って暮らす人魚です。元来大人しい性格で戦闘能力はかなり低めですが水上にいると力が増します。水属性の特技をいくつか覚えますが魔法は使えません。レベルが上がると水上での能力上昇幅がさらに上がり、水攻撃に対する完全な免疫を得ます。なお、あなたは人魚なので靴を履くことができません。"},
	{TRUE,"赤蛮奇",CLASS_BANKI,RACE_YOUKAI,ENTRY_KISHIN,SEX_FEMALE,
	"あなたはろくろ首の妖怪で、正体を隠して人里で暮らしています。首が伸びるだけではなく頭を胴体から離れて飛ばしたり頭を分身させたり目からレーザーを放ったりと実に多芸です。頭の分身の数と戦闘力はレベルアップで上がり、頭の分身が見たものは本体も認識することができます。本体の戦闘能力は低くはありませんが妖怪としてはごく平凡です。開けた場所で分身に援護させつつ直接攻撃するスタイルが基本になります。騎乗に適性を持ちますがそれ以外の技能は低めです。あなたは首飾りを装備できません。"},
	{TRUE,"今泉　影狼",CLASS_KAGEROU,RACE_WARBEAST,ENTRY_KISHIN,SEX_FEMALE,
	"あなたは珍しいニホンオオカミ女で、普段は迷いの竹林に住んでいます。比較的高い身体能力を持ちますが武器の扱いは苦手で、基本的に格闘で戦います。また身のこなしは俊敏なものの手先はあまり器用なほうではありません。変容・暗黒いずれかの領域の魔法を習得でき、難しい魔法は使えませんが一部の変身魔法は大得意です。艶やかなロングヘアが自慢で、髪が傷む火炎・電撃攻撃は嫌いです。満月の晩には身体能力がさらに上昇し、強力な狼に変身可能になります。変身時は装備品の一部が無効化されるので耐性には気を付けてください。"},
	{TRUE,"九十九　弁々",CLASS_BENBEN,RACE_TSUKUMO,ENTRY_KISHIN,SEX_FEMALE,
	"あなたは古い琵琶の付喪神です。自分で自分自身を演奏することができ、様々な効果をもつ音楽を奏でて戦います。特に敵を直接攻撃するような音楽を好み、ある程度の近接戦闘もこなせます。また魔法を一領域習得することが可能です。あなたは魔法を詠唱したり薬を飲みながらでも演奏ができますが、隣接攻撃や装備変更など長時間手を使うことをすると演奏が途切れます。また演奏の邪魔になるグローブ類を装備できません。あなたの本体は琵琶のため通常の食事では腹を満たせず、魔法の道具から充填魔力を吸収して腹を満たします。毒や盲目、轟音攻撃に耐性を持ちますが破邪と劣化攻撃はあなたの大敵です。"},
	{TRUE,"九十九　八橋",CLASS_YATSUHASHI,RACE_TSUKUMO,ENTRY_KISHIN,SEX_FEMALE,
	"あなたは古い琴の付喪神です。自分で自分自身を演奏することができ、様々な効果をもつ音楽を奏でて戦います。特に搦め手で敵を惑わせるような音楽を好み、肉弾戦は苦手ですが魔法を二領域習得することが可能です。あなたは魔法を詠唱したり薬を飲みながらでも演奏ができますが、隣接攻撃や装備変更など長時間手を使うことをすると演奏が途切れます。また演奏の邪魔になるグローブ類を装備できません。あなたの本体は琴のため通常の食事では腹を満たせず、魔法の道具から充填魔力を吸収して腹を満たします。毒や盲目、轟音攻撃に耐性を持ちますが破邪と劣化攻撃はあなたの大敵です。"},
	{TRUE,"鬼人　正邪",CLASS_SEIJA,RACE_YOUKAI,ENTRY_KISHIN,SEX_FEMALE,
	"あなたは反逆の魂を持つ天邪鬼です。ある大事件を起こしてお尋ね者になり、幻想郷の住人全てから追われています。あなたの身体能力は極めて貧弱なため、敵に肉弾戦を挑むのは無謀です。どこかに隠し持っている大量の道具を駆使して戦います。道具には「魔道具」と「不思議道具」があり、どちらも探索拠点か宿屋を利用することで使用回数が回復します。魔道具に限ってはダンジョン内で充填することもできます。継続戦闘能力に欠けるため今の手持ち道具で倒せる敵を冷静に見極める必要があります。またあなたは極度の捻くれ者のため、人に感謝されると精神に打撃を受けて能力が低下します。"},
	{TRUE,"少名　針妙丸",CLASS_SHINMYOUMARU,RACE_GREATER_LILLIPUT,ENTRY_KISHIN,SEX_FEMALE,
	"あなたは一寸法師の末裔であり、伝説の打出の小槌の正当な継承者です。打出の小槌に願い事をすることで様々な有利なことを起こせますが、小槌に何かを願うと次に何かを願えるようになるまで一定時間の小槌の充填が必要です。また小槌の力を使うと様々な代償が発生することがあり、大きな願いであるほど強大な代償になります。魔法は使えませんが隠密や魔道具などの技能は高く経験を積むのも早めです。"},
	{TRUE,"堀川　雷鼓",CLASS_RAIKO,RACE_TSUKUMO,ENTRY_KISHIN,SEX_FEMALE,
	"あなたは聡明さと大胆さを併せ持つ強力な付喪神です。装備した武器を使い込むことで武器の力を引き出してパワーアップさせ、さらに武器の能力に応じた魔法やブレスなどを発動することができるようになります。ただし固定アーティファクトの武器は個性が強すぎるせいかこの能力の対象になりません。またあなたは楽器の付喪神なので、自らを演奏して様々な効果を持つ音楽を奏でることができます。手だけでなく足でも演奏できるため、通常攻撃やアイテムの使用などで音楽が中断されません。総合的に極めて高い能力を持ち成長も比較的早めですが、スコアは大幅に下がります。"},

	{TRUE,"清蘭",CLASS_SEIRAN,RACE_GYOKUTO,ENTRY_KANZYU,SEX_FEMALE,
		"あなたは月から派遣されてきた地上調査部隊「イーグルラヴィ」の一員です。潜入捜査の任務にあたっており、探索と銃が得意です。あなたは異次元から弾丸を飛ばすことができ、銃に関する特技をいくつか習得します。特技の使用には主に知能か器用さが問われます。接近戦能力、開錠などの技能はごく平凡です。中盤以降を切り抜けるには強力な銃を手に入れる必要があるでしょう。"},
	{TRUE,"鈴瑚",CLASS_RINGO,RACE_GYOKUTO,ENTRY_KANZYU,SEX_FEMALE,
		"あなたは月から派遣されてきた地上調査部隊「イーグルラヴィ」の一員です。情報管理の任務にあたっており、探索と調査能力に優れています。あなたの基本的な戦闘能力は平凡ですが、団子を食べれば食べるほどパワーアップするという不思議な能力を持っています。ここぞというときには惜しみなく団子を食べて切り抜けましょう。団子を食べ過ぎて減速することはありません。またダンジョンの探索中には時々仲間が支援物資として団子を届けてくれます。"},
	{TRUE,"ドレミー・スイート",CLASS_DOREMY,RACE_YOUKAI,ENTRY_KANZYU,SEX_FEMALE,
	"あなたは獏の妖怪で、人の悪夢が大好物です。通常の食物ではほとんど栄養を得られず、ダンジョンや宿屋などで眠っている者の夢を食べて腹を満たします。敵を眠らせたり精神を攻撃する特技を習得し、また魔法を一領域習得可能です。あなたの戦闘能力は高いとは言えませんが、あなたの本領は夢の世界で発揮されます。ダンジョン「夢の世界」の中では能力が上昇して特技が大幅に強化され、さらに特殊な技で敵を倒すことでその敵に変身することができます。"},
	{TRUE,"稀神　サグメ",CLASS_SAGUME,RACE_LUNARIAN,ENTRY_KANZYU,SEX_FEMALE,
	"あなたは月の都の重鎮で、幻想郷にオカルトを具現化させた張本人です。あなたが何かについて語ると望む望まざるに関わらずその事態が逆転します。巻物を読むと書かれていることと全く違うことが起こるので気を付けてください。あなたは一切の穢れを拒絶する月の民なので穢れを含む攻撃は大の苦手です。"},
	{TRUE,"クラウンピース",CLASS_CLOWNPIECE,RACE_FAIRY,ENTRY_KANZYU,SEX_FEMALE,
	"あなたは地獄の女神に仕える妖精でしたが、主の友人の力で「純化」されて覚醒し、生命力の塊となりました。今のあなたは完全に妖精の枠から外れた強大な力を持っていますが、この力はあくまで一時的に与えられたものであり、失われた体力は二度と回復することはありません。完全に体力がなくなると帰ってふて寝してしまいゲームオーバーです。あなたは地獄の松明を携えており、これを使うことで自分や仲間、さらには周囲の敵を狂わせたり強化することができます。"},
	{TRUE,"純狐",CLASS_JUNKO,RACE_DEITY,ENTRY_KANZYU,SEX_FEMALE,
	"あなたは古く強力な仙霊です。しかしあなたの核をなすのは、遠い昔に復讐を遂げて今なお消えない強烈な憎悪です。今の所その憎しみは月の民に向けられており、彼らを見かけると叩き潰さずにはおれません。あなたは万物がかつて持っていた本質的な力を保持して利用する「純化」という能力を持ち、この能力によって様々な奇想天外なことを成し遂げることができます。"},
	{TRUE,"ヘカーティア・ラピスラズリ",CLASS_HECATIA,RACE_DEITY,ENTRY_KANZYU,SEX_FEMALE,
	"あなたは地球と月と異界の地獄を統べる女神です。それぞれの地獄にそれぞれ体を持っており、今冒険している身体を他の身体と交代させることができます。性格、装備品、HP、MP、突然変異、使える魔法領域は体ごとに別々になります。あなたの頭の上には大きな球体が載っており、ヘルメットを装備できません。"},

	{TRUE,"エタニティラルバ",CLASS_LARVA,RACE_FAIRY,ENTRY_TENKUU,SEX_FEMALE,
		"あなたはアゲハ蝶の妖精で、成長と再生と変態(生物学的な)の具現でもあります。初期の能力は哀れなほどに低いのですが成長がとても早く、成長に従って能力が大幅に上昇し様々な突然変異を獲得します。あなたはいくつ突然変異を持っても再生能力が落ちないので、この不安定な潜在能力をいかに活用するかが問われます。あなたは非力な妖精のため、あなたの冒険はかなりの苦労を強いられることになるでしょう。"},
	{TRUE,"坂田　ネムノ",CLASS_NEMUNO,RACE_YOUKAI,ENTRY_TENKUU,SEX_FEMALE,
		"あなたは妖怪の山で独自の生活を営む山姥です。縄張りを荒らす者には容赦をしません。ダンジョンの中で誰もいない部屋を聖域に設定し、そこで敵を迎え撃つときにこそあなたの力は最も発揮されます。聖域から出ているときのあなたはそれほど強いわけではないので、敵地に攻め込まざるを得ない状況への対処は苦手です。"},
	{TRUE,"高麗野 あうん",CLASS_AUNN,RACE_WARBEAST,ENTRY_TENKUU,SEX_FEMALE,
		"あなたは狛犬です。神社やお寺に勝手に住み着いて陰ながら守護しています。狛犬なので他の獣人ほど素早くありませんが体が丈夫で破邪攻撃を弱点としません。また仙術の心得も少しは持っています。あなたの頭には大きな角が生えており、それを使って強力な攻撃をすることができますがその代わりに兜をかぶることができません。実はあなたは体を二つ持っており、一部の特技を使うと二人共が一箇所に現れて一緒に戦います。"},
	{TRUE,"矢田寺 成美",CLASS_NARUMI,RACE_MAGIC_JIZO,ENTRY_TENKUU,SEX_FEMALE,
		"あなたは魔法の森の魔力で生命を得て動き出したお地蔵さんです。衆生の救済を自らの使命と考えていますが、出不精でほとんど森から出ようとしません。あなたは生命操作の魔法を最も得意としており、魔力の流れにも敏感です。人里離れた森で風雪に耐えてきたため冷気攻撃に強い耐性を持ちます。"},
	{TRUE,"爾子田 里乃",CLASS_SATONO,RACE_DEMIGOD,ENTRY_TENKUU,SEX_FEMALE,
		"あなたは秘神「摩多羅 隠岐奈」の配下で、主な仕事は誰かの背後で踊り狂うことです。あなたはモンスターをスカウトして背後で踊ることでそのモンスターの精神力を強化することができます。背後で踊るあなたはそのモンスターに騎乗しているのに近い扱いになります。乗馬可能なモンスターでなくても背後で踊ることができます。強力なモンスターをスカウトできれば一気に戦力が強化されることでしょう。しかしあなた自身の戦闘能力はそれほど高くありません。" },
	{TRUE,"丁礼田 舞",CLASS_MAI,RACE_DEMIGOD,ENTRY_TENKUU,SEX_FEMALE,
		"あなたは秘神「摩多羅 隠岐奈」の配下で、主な仕事は誰かの背後で踊り狂うことです。あなたはモンスターをスカウトして背後で踊ることでそのモンスターの身体能力を強化することができます。背後で踊るあなたはそのモンスターに騎乗しているのに近い扱いになります。乗馬可能なモンスターでなくても背後で踊ることができます。強力なモンスターをスカウトできれば一気に戦力が強化されることでしょう。しかしあなた自身の戦闘能力はそれほど高くありません。"},
	{TRUE,"摩多羅 隠岐奈",CLASS_OKINA,RACE_DEITY,ENTRY_TENKUU,SEX_FEMALE,
		"あなたは長きにわたり闇の奥に秘されてきた謎めいた神様です。不思議な後戸の力でこれまで幻想郷を裏から支えてきました。あなたはあちこちのモンスターの背中の扉を通じて力を吸収し続けており、レベルアップに伴いモンスター達の特技を我が物とします。またあなたは後戸の神様の他にも様々な神様の顔を持っていて、やや雑多ながらも強力ないくつかの特技を習得します。あなたは闇の奥を真昼のように見通し、閉じた扉のある地形をまるで扉などないかのように歩むことができます。"},

	{ TRUE,"戎 瓔花",CLASS_EIKA,RACE_STRAYGOD,ENTRY_KIKEI,SEX_FEMALE,
		"あなたは神格化しつつある水子霊で、賽の河原で子供達の霊と一緒に石を積んで暮らしています。あなたの特技は石積みです。あなたが祈りを込めて積んだ石は神聖な力を発揮します。しかしあなた自身は戦う力をほとんど持っていません。あなたの冒険はまさに石積みのような苦行となることでしょう。あなたは経験を積むことで神格として成熟し能力が大きく変わります。" },
	{ TRUE,"牛崎 潤美",CLASS_URUMI,RACE_YOUKAI,ENTRY_KIKEI,SEX_FEMALE,
		"あなたは三途の河で古代魚を捕って暮らす牛鬼です。あなたには身近な物の重さを変える能力があり、得意技は石の赤子を押し付けて超重量にすることでモンスターを水に沈めることです。あなたは水辺でないと本領を発揮することができませんが、どのような場所でも高い身体能力を活かしてそれなりに戦うことができるでしょう。" },
	{ TRUE,"庭渡 久侘歌",CLASS_KUTAKA,RACE_DEITY,ENTRY_KIKEI,SEX_FEMALE,
		"あなたはニワタリ神という鶏の神様です。普段は彼岸の関所で門番を勤めています。あなたには喉の病気を癒す能力があり、毒に対する耐性があります。また周囲のものを感知したり招かれざる者を退去させるような特技を習得し、魔法を一領域学習可能です。あなたの頭の上には可愛らしいヒヨコが住んでおり、ピンチのときにはピヨピヨ鳴いて危機を教えてくれます。" },
	{ TRUE,"吉弔 八千慧",CLASS_YACHIE,RACE_ANIMAL_GHOST,ENTRY_KIKEI,SEX_FEMALE,
		"あなたは畜生界の巨大組織である鬼傑組（きけつぐみ）の組長です。逆らう気力を失わせる能力を持っており、勧誘によってユニークモンスターを配下にすることができます。他にも敵の動きを妨害したり配下を支援する特技を習得します。あなたは謀略を巡らせることを好みますが肉弾戦もけして苦手ではありません。ただし立派な角と尻尾と甲羅が邪魔で鎧や兜などの重装備をすることができません。" },
	{ TRUE,"杖刀偶 磨弓",CLASS_MAYUMI,RACE_HANIWA,ENTRY_KIKEI,SEX_FEMALE,
		"あなたは造形神『埴安神　袿姫』が創り上げた埴輪兵団の兵長です。埴輪の兵士を召喚する特技を習得し、低いMPコストで多くの埴輪の配下を維持できます。あなた自身の戦闘能力も非常に高く、とくに剣・弓・騎乗の技能に高い適性をもちます。またあなたには忠誠心を力にする能力があり、この能力により腕力と耐久力が賢さの数値より低くなることがありません。あなたの見た目はほとんど人間と変わりませんが、あなたの躰はがらんどうの埴輪です。そのため様々な種類の攻撃に対して通常の生物には有り得ないほどの抵抗力を持ちますが、轟音や破片の攻撃は苦手です。" },
	{ TRUE,"埴安神 袿姫",CLASS_KEIKI,RACE_DEITY,ENTRY_KIKEI,SEX_FEMALE,
		"あなたは畜生界の虐げられし人間霊たちの祈りに応えて現れた造形神です。普段は銅鐸や鏡や埴輪の製作に勤しんでいますが、時には自ら前線に出て戦うこともあります。	あなたは武器防具や装飾品からその能力(エッセンス)を抽出し、それを使って他の装備品を強化したり一部の特技を発動することができます。また魔法を一領域巧みに扱うことができ、近接戦闘の能力も低くはありません。ある程度の準備を整えればどのような敵が相手でも有利に戦いを進めることができるでしょう。" },
	{ TRUE,"驪駒 早鬼",CLASS_SAKI,RACE_ANIMAL_GHOST,ENTRY_KIKEI,SEX_FEMALE,
		"あなたは畜生界の巨大組織である勁牙組（けいがぐみ）の組長です。	あれこれ計略を巡らせることは好まず、行く手に立ちふさがるものは全て蹴散らすのがあなたの流儀です。あなたは強靭な脚力によって非常に素早く動くことができ、またモンスターの攻撃によって減速を受けることがありません。格闘と銃が得意でそれにかかわるいくつかの特技を習得します。動物霊であるにも関わらず閃光を弱点としませんが、破邪攻撃はやはり苦手です。" },

	{ TRUE,"豪徳寺　ミケ",CLASS_MIKE,RACE_WARBEAST,ENTRY_KOURYUU,SEX_FEMALE,
		"あなたは三毛猫の招き猫です。白猫主義の招き猫社会から排斥された過去を持ち、今は妖怪の山で気楽に暮らしています。あなたは招き猫として客や金を呼び寄せる能力を得るはずでしたが、十分な修行ができなかったせいでデメリットを伴う癖のある能力になってしまいました。状況を見極めて賢く能力を使う必要があります。あなたはアビリティカードに目をつけ流行に乗って一儲けしようと目論んでいます。しかし招き猫社会の妨害によりカード販売所には立ち入ることができません。カード交換所、カードを持つ人妖との交渉、偽天棚の賭場など他の手段でカードを手に入れる必要があります。前途は多難ですが強かにやり抜きましょう。" },
	{ TRUE,"山城　たかね",CLASS_TAKANE,RACE_YAMAWARO,ENTRY_KOURYUU,SEX_FEMALE,
		"あなたは交渉事が大得意な山童です。最近出回り始めた「アビリティカード」に目をつけカード売人として一旗揚げることにしました。しかし他の妖怪たちも先を争ってアビリティカードを買い集め始めており高騰は必至です。一刻も早くお金を貯めてカードを買い漁りましょう。あなたは背中に大型のカードケースを背負っており、この中にカードを16種類まで収納することができます。さらに森の気を操る能力をもち、森に関係する幾つかの特技を習得します。しかし魔法を学習することはできず身体能力もあまり高くありません。" },
	{ TRUE,"駒草　山如",CLASS_SANNYO,RACE_YOUKAI,ENTRY_KOURYUU,SEX_FEMALE,
		"あなたは妖怪の山で賭場を開帳する「山女郎」という妖怪です。特別なタバコを吸うことで周囲のモンスターに朦朧や混乱などの効果を与える能力を持っています。一度タバコを吸い始めるとその効果は毎ターン継続的に発動し、そのたびにMPを少し消費します。また賭場の主なので腕っ節には多少の覚えがあります。扇を使った戦闘術もお手の物です。あなたは「アビリティカード」を使った賭けで一儲けするためにカードを仕入れようとしています。しかし競争相手が次第に増え始めており、カードの価格は高騰を始めています。急いでお金を稼いでカードを買いあさりましょう。" },
	{ TRUE,"玉造　魅須丸",CLASS_MISUMARU,RACE_DEITY,ENTRY_KOURYUU,SEX_FEMALE,
		"あなたは勾玉制作職人です。鉱物などの素材から勾玉を制作し、これを専用スロットに装備して発動することで様々な能力を発揮することができます。使用する素材によって勾玉に発現する能力の種類や強さが変化します。またあなたは街の鍛冶宝飾ギルドで宝石や装飾品からエッセンスを抽出しそれを別の装飾品に付与することができます。さらに魔法を一領域習得することができますが、肉弾戦は全くの不得手です。また勾玉の繊細な力を使うため劣化攻撃が苦手です。" },
	{ TRUE,"菅牧　典",CLASS_TSUKASA,RACE_YOUKO,ENTRY_KOURYUU,SEX_FEMALE,
		"あなたは大天狗に仕える管狐です。人に囁きかけて唆し破滅と混乱をもたらすことをこよなく好みます。あなたは配下モンスターの背後に隠れて操る「寄生」という特殊な騎乗状態になることができます。通常騎乗可能なモンスター以外にも寄生ができ、他のモンスターからの攻撃は高確率で配下モンスターが受け、また配下モンスターが戦うことで得た経験値やアイテムは全てあなたが横取りします。あなたの肉弾戦能力は全く話になりませんが代わりに魔法を一領域習得することができます。配下モンスターを盾にしつつ後ろから魔法や特技で攻撃するのが基本的な戦い方になります。変身はあまり得意ではないようです。あなたは服が汚れるのが嫌いで、劣化や汚染の攻撃で通常より多くのダメージを受けてしまいます。" },
	{ TRUE,"飯綱丸　龍",CLASS_MEGUMU,RACE_KARASU_TENGU,ENTRY_KOURYUU,SEX_FEMALE,
		"あなたは妖怪の山で鴉天狗たちを束ねる大天狗です。非常に動きが素早く機知に富み文武両道の強さです。さらに視界内すべてを攻撃する強力な特技を複数習得します。ただし長く生きてきたためレベルアップに必要な経験値は多くなります。また頭には頭襟しか装備できません。" },
	{ TRUE,"天弓　千亦",CLASS_CHIMATA,RACE_DEITY,ENTRY_KOURYUU,SEX_FEMALE,
		"あなたは市場の神です。しかしあなたが司る非日常の場としての市場はとうの昔に廃れ、あなたは力を失い消え入りそうになっています。そんなときにある大天狗が起死回生のビジネススキームを持ち込んできました。これが成功すればあなたは再びかつての力を取り戻すことができるでしょう。そのためには幻想郷中の人妖に接触して能力をコピーし、アビリティカードとして流通させなければなりません。能力のコピーには知能と器用さが必要です。コピーする前に倒してしまうとその者の能力は二度と手に入らないので気をつけましょう。最初期のあなたは隠密能力だけは高いのですが他の能力は低めです。多くのカードが流通するとあなたの能力と特技は強力になり、代わりに隠密能力は低下します。" },
	{ TRUE,"姫虫　百々世",CLASS_MOMOYO,RACE_DAIYOUKAI,ENTRY_KOURYUU,SEX_FEMALE,
		"あなたは凶悪な大蜈蚣(オオムカデ)の大妖怪です。非常に高い肉体能力と戦闘系技能をもち、様々な攻撃や防御の特技を習得し、さらにレベルが上がると攻撃に毒属性が追加されます。またつるはしを装備すると武器の威力にボーナスがつきます。一対一の殴り合いならほぼ敵なしでしょう。ただし隠密や探索はからっきしです。中盤以降はなるべく周囲に注意を払いましょう。あなたは食事コマンドで鉱石を食べて一時的に特別な効果を得ることができます。" },

	{ TRUE,"孫　美天",CLASS_BITEN,RACE_YOUKAI,ENTRY_JUUOUEN,SEX_FEMALE,
		"あなたはほんの少し前までただの山猿でしたが、ある鬼にそそのかされて聖地の霊を喰ったことで妖怪化しました。その後紆余曲折を経て畜生界の動物霊の暴力組織「鬼傑組」の遊撃隊に所属しています。もとが猿だけあって森での立ち回りはお手の物です。戦闘においては棒術と投擲を得意とします。さらに仙術を一領域習得することができますがこちらの適性はあまり高くありません。" },
	{ TRUE,"三頭　慧ノ子",CLASS_ENOKO,RACE_WARBEAST,ENTRY_JUUOUEN,SEX_FEMALE,
		"あなたは破戒僧だった頃の日白残無の肉を食らって永遠の呪いを受けたヤマイヌです。両腕に物騒なトラバサミが取り付けられており、これによって強力な格闘攻撃ができますが代わりに武器や盾を装備することができなくなります。薬や巻物などの消耗品は問題なく使用できます。またあなたは罠の扱いを得意としており、罠を設置して敵を罠にかけることができます。敵と対峙する前にダメージを与えたり弱体化させるなどのトリッキーな戦い方が可能です。あなたは鬼の宝玉を首からかけています。この宝玉はあなたに大きな力をもたらしますが、これを装備していると周りの人妖が宝を狙って襲いかかってきます。" },
	{ TRUE,"天火人　ちやり",CLASS_CHIYARI,RACE_YOUKAI,ENTRY_JUUOUEN,SEX_FEMALE,
		"あなたは「天火人」と呼ばれる吸血妖怪です。旧血の池地獄に長い間引き籠もっていましたが最近外に出てきました。あなたは通常の食事が口に合わず、代わりに特技で他者から血を奪ってそれを飲むことで腹を満たして体力と魔力を回復します。あなたは呪われた血が好みのためゾンビやデーモンの血でも気にしません。またあなたは自らの血を使って攻撃力を大幅に強化する特技を持っています。使いすぎて出血ダメージで倒れないように気をつけましょう。" },
	{ TRUE,"豫母都　日狭美",CLASS_HISAMI,RACE_DEATH,ENTRY_JUUOUEN,SEX_FEMALE,
		"あなたは地獄の住人の黄泉醜女(ヨモツシコメ)です。日白残無の部下として地獄の案内人をしたり裁きを待つ魂を勝手に地獄に誘ったりして暮らしています。戦闘においては魔法を主体に戦います。また敵を拘束したり特定の敵をどこまでも追いかけ回すような特技を習得します。あなたは甘味を食べるとMPを回復することができます。" },
	{ TRUE,"日白　残無",CLASS_ZANMU,RACE_ONI,ENTRY_JUUOUEN,SEX_FEMALE,
		"あなたは地獄を支配し大改革をもたらした鬼です。かつては人間の破戒僧でした。あなたの高い知性は現在過去未来の様々なものを見通します。戦闘においては肉弾戦よりは魔法や投擲などを得意とします。魔法はあらゆる大領域から二領域を習得可能です。" },

	{TRUE,"伊吹　萃香",CLASS_SUIKA,RACE_ONI,ENTRY_TASOGARE,SEX_FEMALE,
	"あなたは幻想郷で長らく忘れられていた強大な鬼です。小柄ながら最高クラスの身体能力を持ち格闘と鈍器を得意とします。密と疎を操る能力を持ち、巨大化したり霧になったりアイテムを集めるなど多彩な技を習得します。あなたはいくら酒を飲んでも倒れませんが、酔いが醒めると能力が大幅に低下してしまいます。"},
	{TRUE,"永江　衣玖",CLASS_IKU,RACE_YOUKAI,ENTRY_TASOGARE,SEX_FEMALE,
	"あなたは竜宮の使いと呼ばれる妖怪です。龍神様に仕え、普段は雲海の中を漂って暮らしています。空気を読む能力を持ち、ダンジョンの危険度やアイテムの価値を素早く察したり敵の攻撃を受け流すことができます。また雷を操ることが得意です。武器や魔法はそれなりに色々使えますが、羽衣を用いた格闘を最も得意とします。なお、羽衣は天帝様からの支給品のため壊れたら大変なことになります。"},
	{TRUE,"比那名居　天子",CLASS_TENSHI,RACE_TENNIN,ENTRY_TASOGARE,SEX_FEMALE,
	"あなたはとある名家が天人となったときついでに天人になった元人間で、素行が悪く天界で不良天人と呼ばれています。しかしあなたの実力は本物で、大地に働きかける様々な強力な特技を使いこなします。あなたの愛用の武器は天界の宝剣である「緋想の剣」です。一部の特技はこの剣を装備していないと使えません。"},
	{TRUE,"秦　こころ",CLASS_KOKORO,RACE_TSUKUMO,ENTRY_TASOGARE,SEX_FEMALE,
	"あなたは仮面の付喪神で面霊気とも呼ばれます。あなたの本体は千年以上前に高名な聖人に作られた六十六の面で、そのためその辺の付喪神に比べ明らかに強い力を持ちます。ある事件をきっかけに豊かな感情を(無表情のまま)表すようになり、現在は感情を安定させるための修行と見聞の日々を過ごしています。人の感情を操る力を持ち、敵を恐れさせたり混乱させることが得意です。敵を特殊な技で倒すことでその敵の感情を表す面を作成し、それを装備して戦うことでその敵の技を再現することができます。ただしあなたは通常の頭防具を装備できません。"},
	{TRUE,"少名　針妙丸",CLASS_SHINMYOU_2,RACE_KOBITO,ENTRY_TASOGARE,SEX_FEMALE,
	"あなたは一寸法師の末裔であり、伝説の打出の小槌の正当な継承者です。今回は空を飛ぶ不思議なお椀に乗って戦います。あなたは人の膝くらいの身長しかない小人なので、肉体能力は極めて貧弱で重いものを装備できません。高い技能と敏捷性、そして様々なトリッキーな特技を使いこなして活路を開きましょう。"},
	{TRUE,"宇佐見　菫子",CLASS_SUMIREKO,RACE_JK,ENTRY_TASOGARE,SEX_FEMALE,
	"あなたは外界の女子高生で超能力者です。好奇心から幻想郷へ結界破りを仕掛けようとしたものの幻想郷の人妖に阻止され、今は外界で眠っている間に夢の中で幻想郷へ訪れています。mコマンドから通常職業の超能力者と同様の超能力を使用でき、さらにJコマンドからも独自の超能力を使用できます。独自の能力は投擲に関するものが多く攻撃に向いています。一方、あなたの身体能力は普通の人間そのもので武器の心得も全くありません。そのため通常職業の超能力者より肉弾戦が苦手です。"},

	{ TRUE,"依神　女苑",CLASS_JYOON,RACE_DEITY,ENTRY_TASOGARE,SEX_FEMALE,
		"あなたは疫病神です。アイテムや補助魔法など多くのものを持つ敵はあなたの餌食となることでしょう。成金趣味のあなたは両手の指に8個もの指輪を装備することができますが、指輪のパラメータは半減してしまいます。あなたは格闘攻撃をするときに他者から金を奪取することができ、また時には奪った財を派手に散財しながら戦うこともできます。" },
	{ TRUE,"依神　紫苑",CLASS_SHION,RACE_DEITY,ENTRY_TASOGARE,SEX_FEMALE,
		"あなたは誰からも嫌われる貧乏神です。大変な借金を抱えており、実質的に店や街の施設を利用することができません。さらに呪われていない装備品は即座に差し押さえられ半日ほどで没収されてしまいます。あなたがダンジョンで普通に戦うことは極めて困難で、「強制完全憑依」の特技を使ってモンスターの体を乗っ取って戦うことになります。あなたは周囲で起こった不幸を少しずつその身に蓄積していき、貯め込んだ不幸を開放してとてつもなく凶悪な「スーパー貧乏神」に変身することができます。" },
	{ TRUE,"饕餮　尤魔",CLASS_YUMA,RACE_DEITY,ENTRY_TASOGARE,SEX_FEMALE,
		"あなたは畜生界で剛欲同盟を率いる同盟長です。謀略と肉弾戦の両方に秀で、とくに耐久力に優れます。あらゆる物を食べることができ、食べたものによって栄養だけでなく耐性、能力、特技、経験値、MPなど様々なものを獲得します。あなたは通常の十倍の満腹度限界をもちますが腹が減る速度がそれ以上に早く、さらに隣接攻撃や特技の使用によっても満腹度を大幅に消費します。なるべく腹が減らない行動を心がけましょう。もし腹が減って倒れてしまうと操作不能になり周囲のあらゆるものを無差別に吸い込み始めます。何も吸い込むものがない場合ゲームオーバーになるまで止まりません。" },

	{TRUE,"光の三妖精",CLASS_3_FAIRIES,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
	"あなたたちは光の三妖精と呼ばれる三人組の妖精です。	『サニーミルク』は光を屈折させる能力を、『ルナチャイルド』は音を消す能力を、『スターサファイア』は周囲の生物を探る能力を持ちます。	三人一緒にダンジョンに挑み、三人合わせて武器を2つ、盾を1つ、光源を1つ、防具やアクセサリを8個まで装備可能です。ただし同部位の装備品は最大3つまでとなります。	二刀流や近接・射撃武器同時装備のペナルティは発生しません。"},
	{TRUE,"サニーミルク",CLASS_SUNNY,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
	"あなたは「光の三妖精」と呼ばれる妖精三人組の一人で、陽光の妖精です。いつも元気で行動力があり、その場の思いつきで他の二人を引っ張り回します。あなたは光を屈折させる能力を持っており、敵を幻影で惑わせたり光線で攻撃することができます。戦闘においては比較的肉弾戦に向いており、魔道具や飛び道具の扱いは苦手です。あなたは非力な妖精のため、あなたの冒険はかなりの苦労を強いられることになるでしょう。"},
	{TRUE,"ルナチャイルド",CLASS_LUNAR,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
	"あなたは「光の三妖精」と呼ばれる妖精三人組の一人で、月光の妖精です。妖精には珍しく、静かに本を読んで過ごすことを好みます。あなたは周囲の音を消す能力を持っており、魔法の詠唱を妨害することができます。戦闘においては肉弾戦はあまり得意ではなく、魔法や特技などを織り交ぜてだましだまし戦う必要があります。ときどきうっかり転んで武器を落とすので気を付けてください。あなたは非力な妖精のため、あなたの冒険はかなりの苦労を強いられることになるでしょう。"},
	{TRUE,"スターサファイア",CLASS_STAR,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
	"あなたは「光の三妖精」と呼ばれる妖精三人組の一人で、星の光の妖精です。普段はほかの二人が騒ぐのを楽しそうに見ています(そして危険が迫ると真っ先に逃げます)が、時には驚くような行動力を発揮することがあります。あなたは近くの生物の場所を探る能力を持っており、レベルが上がると感知できるものの種類や範囲が増えます。戦闘においては肉弾戦は苦手で、魔道具や遠隔攻撃、罠などを駆使して戦う必要があります。あなたは非力な妖精のため、あなたの冒険はかなりの苦労を強いられることになるでしょう。"},
	{TRUE,"茨木　華扇",CLASS_KASEN,RACE_SENNIN,ENTRY_OTHER,SEX_FEMALE,
	"あなたは動物達を導くことを使命とする新米の仙人です。動物を使役し乗りこなす能力に長けています。新米仙人の割になぜか異様に身体能力が高く、なぜかヘルメットを装備できず、なぜか破邪攻撃が弱点です。常に包帯を巻いている右手はいくつかの秘められた力を持っています。"},
	{TRUE,"姫海棠　はたて",CLASS_HATATE,RACE_KARASU_TENGU,ENTRY_OTHER,SEX_FEMALE,
	"あなたは鴉天狗の新聞記者で、射命丸文とはライバル関係です。鴉天狗の割にはやや敏捷性に欠けますが念写の能力を持っており感知や探索に優れます。接近戦よりは射撃に適性を持ち、生命・暗黒・予見のいずれかの領域の魔法をある程度習得可能です。"},
	{TRUE,"綿月　依姫",CLASS_YORIHIME,RACE_LUNARIAN,ENTRY_OTHER,SEX_FEMALE,
	"あなたは月の名家の姫で、豊姫の妹です。八百万の神をその身に降ろす力を持ち、あらゆる神様の力を片端から行使します。月の防衛隊の指揮官兼教官でもあり、接近戦の技能も卓越しています。あなたは穢れを嫌う月の民であり、様々な邪悪な攻撃で大ダメージを受けてしまいます。"},
	{TRUE,"レイセン",CLASS_REISEN2,RACE_GYOKUTO,ENTRY_OTHER,SEX_FEMALE,
	"あなたは月の兎です。来る日も来る日も餅を搗く生活が嫌になって地上に逃げ出しましたが、その後色々あって月の防衛隊の一員に納まりました。戦闘に関するセンスはあまり良くないようですが、時には目にも止まらぬ俊敏な動きをします。"},
	{TRUE,"本居　小鈴",CLASS_KOSUZU,RACE_HUMAN,ENTRY_OTHER,SEX_FEMALE,
		"あなたは人里の貸本屋の娘で、あらゆる文字を読み解く能力を持っています。自らが信ずるべき真実を求め、秘蔵の商品を鞄に詰めて店を飛び出しました。しかしあなた自身の戦闘能力はとても里の外の脅威に対抗できるようなものではありません。あなたの冒険は過酷なものとなるでしょう。"},
	{TRUE,"クラウンピース",CLASS_VFS_CLOWNPIECE,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
		"あなたは地獄出身の妖精です。少し前の異変で重要な働きをこなし、その後は主からの密命を受けて地上で暮らしています。妖精でありながら地獄で頭角を現すほどの力を持っていますが、頭の中身は普通の妖精並みです。あなたは手に持った松明で人々を狂わせることができ、また地獄出身だけあって火炎と地獄の属性攻撃に対する耐性があります。" },

	{ TRUE,"奥野田　美宵",CLASS_MIYOI,RACE_ZASHIKIWARASHI,ENTRY_OTHER,SEX_FEMALE,
		"あなたは伊吹萃香の酒瓢箪『伊吹瓢』に棲み着く座敷わらしです。瓢箪の影響によるものか人を酔い潰して夢や記憶に干渉する酔魔のような力を持っています。あなたには戦う力はほとんどありませんが、愛想よくお酒を勧めて敵の心すら開かせるほどの接客能力があります。お酒を何度も飲ませてそのまま酔い潰してしまえば倒したのと同じ扱いになります。しかし中には酒を勧めても応じないモンスターやいくら酒を飲んでも酔わないモンスターもいます。そういった敵に襲われてしまったらあなたにできることは逃亡あるのみです。あなたに必要な能力は魅力です。接客の成功率だけでなく飲ませる酒の強さにも影響を与えます。" },

	{ TRUE,"宮出口　瑞霊",CLASS_MIZUCHI,RACE_SPECTRE,ENTRY_OTHER,SEX_FEMALE,
		"あなたは博麗の家に強い怨みをもつ強力な怨霊です。他者に取り憑いて乗っ取る特技を持っており、あなた自身も高い戦闘力を持っています。現在あなたは幻想郷の地上から警戒されており、誰かに憑依しないと地上の建物や店を利用することができません。怨霊であり密かな呪具コレクターでもあるあなたは装備品の呪いの影響を防ぐことができます。しかしあなたは下半身の具現化が不完全なため靴を履くことができず、また首の地獄の首輪を外すことができません。あなたはかつて巫女として様々な術を学んでいましたが、怨霊となったときにほぼ失われています。" },


	{ TRUE,"塵塚　ウバメ",CLASS_UBAME,RACE_YOUKAI,ENTRY_KINJYOU,SEX_FEMALE,
		"あなたは山姥で、聖域に住むほかの山姥たちのリーダー格です。普段は聖域を守るために尽力していますが必要とあれば他者が作った聖域を破壊することもできます。あなたは身体能力が高く肉弾戦を得意としますが魔法を習得することはできません。" },
	{ TRUE,"封獣　チミ",CLASS_CHIMI,RACE_DAIYOUKAI,ENTRY_KINJYOU,SEX_FEMALE,
		"あなたは魑魅(ちみ)と呼ばれる古い物の怪で、自然の力を操って様々な不思議な現象を起こすことができます。しかしあなたは自然のない場所では力を振るうことができず、街や建物など人の手が入った場所では大幅に弱体化してしまいます。とくにクエストダンジョンの攻略は非常に困難になるので十分に力をつけてから挑みましょう。あなたは魔法を一領域習得可能ですが接近戦の適性は低めです。" },
	{ TRUE,"道神　馴子",CLASS_NAREKO,RACE_DEITY,ENTRY_KINJYOU,SEX_FEMALE,
		"あなたははるか昔に何者かに置かれた道祖神が自我を得た存在です。魔法を一領域巧みに扱うことができ、さらに罠や壁を作り出して敵を妨害する特技を持っています。また近接戦闘もそれほど苦手ではありません。あなたは謎掛けが好きで、人に謎をかけて惑わせると精神的に満たされてMPが回復します。" },
	{ FALSE,"ユイマン・浅間",CLASS_YUIMAN,RACE_DEITY,ENTRY_KINJYOU,SEX_FEMALE,
		"" },
	//その他からこっちに持って来た
	{ TRUE,"綿月　豊姫",CLASS_TOYOHIME,RACE_LUNARIAN,ENTRY_KINJYOU,SEX_FEMALE,
	"あなたは月の名家の姫で、依姫の姉です。海と山を繋ぐ力を持ち、大規模な召喚や空間操作を好き放題に行います。荒事は苦手ですが月の最新兵器を所有しており、並みの敵ならまとめて吹き飛ばすことができます。	あなたは穢れを嫌う月の民であり、様々な邪悪な攻撃で大ダメージを受けてしまいます。" },

	{ FALSE,"磐永　阿梨夜",CLASS_ARIYA,RACE_DEITY,ENTRY_KINJYOU,SEX_FEMALE,
		"" },
	{ FALSE,"渡里　ニナ",CLASS_NINA,RACE_YOUKAI,ENTRY_KINJYOU,SEX_FEMALE,
		"" },


};

//Exモードに向かないか向きすぎている職業のスコア倍率に補正(%単位でスコア倍率に乗ずる)
void extra_mode_score_mult(int *mult, int class_idx)
{
	int mod;

	if(!EXTRA_MODE) return;

	switch(class_idx)
	{
	case CLASS_WARRIOR:
		mod = 90;
		break;
	case CLASS_MAGE:
	case CLASS_HIGH_MAGE:
		mod = 120;
		break;

	case CLASS_MINDCRAFTER:
		mod = 90;
		break;
		//v1.1.62 魔法の矢筒を実装したので廃止
//	case CLASS_ARCHER:
//		mod = 120;
//		break;
	case CLASS_MAGIC_EATER:
		mod = 80;
		break;
	case CLASS_ENGINEER:
		mod = 150;
		break;
	case CLASS_CHEMIST:
		mod = 150;
		break;
	case CLASS_CAVALRY:
		mod = 110;
		break;
	case CLASS_TSUKUMO_MASTER:
		mod=125;
		break;
	case CLASS_SH_DEALER:
		mod=125;
		break;
	case CLASS_JEWELER:
	case CLASS_MISUMARU:
		mod=150;
		break;
	case CLASS_NINJA:
		mod=90;
		break;
	case CLASS_TEWI:
		mod=120;
		break;
	case CLASS_KOISHI:
		mod=120;
		break;

	case CLASS_MOMIZI:
		mod=70;
		break;
	case CLASS_SEIGA:
		mod = 110;
		break;
	case CLASS_ORIN:
		mod=70;
		break;
	case CLASS_PATCHOULI:
		mod = 120;
		break;
	case CLASS_MEDICINE:
		mod = 120;
		break;
	case CLASS_YUYUKO:
		mod = 120;
		break;
	case CLASS_LILY_WHITE:
		mod=80;
		break;
	case CLASS_CHEN:
		mod=125;
		break;
	case CLASS_NITORI:
		mod=120;
		break;
	case CLASS_SUWAKO:
		mod=150;
		break;
	case CLASS_SEIJA:
		mod=140;
		break;
	case CLASS_RAN:
		mod=60;
		break;
	case CLASS_SUMIREKO:
		mod=75;
		break;
	case CLASS_KANAKO:
		mod=200;
		break;
	case CLASS_CLOWNPIECE:
		mod = 40;
		break;
	case CLASS_DOREMY:
		mod = 150;
		break;
	case CLASS_HINA:
		mod = 160;
		break;
	case CLASS_SAKUYA:
		mod = 60;
		break;
	case CLASS_3_FAIRIES:
		mod = 80;
		break;
	case CLASS_RAIKO:
		mod = 150;
		break;
	case CLASS_RINGO:
		mod = 80;
		break;
	case CLASS_SAGUME:
		mod = 200;
		break;
	case CLASS_SHION:
		mod = 50;
		break;

	case CLASS_CHIMATA:
		mod = 200;
		break;

	case CLASS_CHIMI:
		mod = 175;
		break;

	default:
		return; //何もしない
	}

	*mult = *mult * mod / 100;

}

static bool get_unique_player(void)
{
	int i, k, n, cs, os;

	int mode = 0;
	cptr str;
	char c;
	int entry;
	unique_player_type table[16];
	int cnt_table;
	char temp[80*12];
	cptr t;
	char    buf[80], cur[80];
	int mul_exp, mul_score;
	char old_name[32];


	while(1)
	{

		bool ok = TRUE;

		put_str("あなたの登場作品を選んでください。(*:ランダム)", 10, 10);

		cs = 0;
		while(1)
		{
			for(n=0;n<ENTRY_MAX;n++)
			{
			//	if(n == cs) c_put_str(TERM_YELLOW, select_unique_entry[n], 12 + n, 10);
			//	else put_str(select_unique_entry[n], 12 + n, 10);
				if(n == cs) c_put_str(TERM_YELLOW, select_unique_entry[n], 12 + n % 16, 10 + (n / 16 * 20));
				else put_str(select_unique_entry[n], 12 + n % 16, 10 + (n / 16 * 20));
			}
			c = inkey();
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);

			if (c == '*')
			{
				k = randint0(ENTRY_MAX);
				break;
			}

			if (c == ' ' || c == '\r' || c == '\n')
			{
				k = cs;
				break;
			}
			if (c == '2' && cs < ENTRY_MAX-1) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == '?')
			{
				show_help("tbirth.txt#Class");
			}

			if (c >= 'a' && c <= 'a'+ENTRY_MAX-1)
			{
				k = c - 'a';
				break;
			}
		}
		clear_from(10);
		entry = k;


		cnt_table = 0;
		for (i = 0; i < UNIQUE_PLAYER_NUM; i++)if (unique_player_table[i].entry == entry)
		{
			if (cnt_table == 16)
			{
				put_str("ERROR:get_unique_player()のcnt_tableが一杯", 12, 10);
				break;
			}
			table[cnt_table++] = unique_player_table[i];
		}
		if (cnt_table == 0)		c_put_str(TERM_L_BLUE, format("作品ID:%dに誰も登録されていない", entry), 8, 10);

		put_str("あなたは誰ですか？(*:ランダム)", 10, 10);
		put_str("(灰字は未実装 ESCで作品選択へ戻る)", 11, 10);

		cs = 0;
		while (1)
		{
			for (n = 0; n < cnt_table; n++)
			{
				if (n == cs)
					c_put_str(TERM_YELLOW, format("%c) %s", ('a' + n), table[n].name), 12 + n, 10);
				//藍・紫はHARD以上限定
				else if ((table[n].class_id == CLASS_RAN || table[n].class_id == CLASS_YUKARI) && difficulty < DIFFICULTY_HARD)
					c_put_str(TERM_L_DARK, format("%c) %s", ('a' + n), table[n].name), 12 + n, 10);
				else if (!table[n].flag_fix)
					c_put_str(TERM_L_DARK, format("%c) %s", ('a' + n), table[n].name), 12 + n, 10);
				else
					put_str(format("%c) %s", ('a' + n), table[n].name), 12 + n, 10);

			}
			Term_gotoxy(0, 0);
			c = inkey();
			if (c == ESCAPE)
			{
				ok = FALSE;
				break;
			}
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);

			//v1.1.97 ランダム選択
			if (c == '*')
			{
				int available_number_list[64];
				int tmp_list_len = 0;

				for (n = 0; n < cnt_table; n++)
				{
					if (!table[n].flag_fix)continue;
					//藍・紫はHARD以上限定
					if (table[k].class_id == CLASS_RAN && difficulty < DIFFICULTY_HARD) continue;
					if (table[k].class_id == CLASS_YUKARI && difficulty < DIFFICULTY_HARD) continue;
					available_number_list[tmp_list_len++] = n;
				}

				if (tmp_list_len)
				{
					k = available_number_list[randint0(tmp_list_len)];
					break;
				}
				//該当ページの実装キャラがまだ一人もいない場合tmp_list_lenが0になり*を押しても何も起こらない
			}


			if (c == ' ' || c == '\r' || c == '\n')
			{
				k = cs;
				if (!table[k].flag_fix)continue;
				//藍・紫はHARD以上限定
				if (table[k].class_id == CLASS_RAN && difficulty < DIFFICULTY_HARD) continue;
				if (table[k].class_id == CLASS_YUKARI && difficulty < DIFFICULTY_HARD) continue;
				break;
			}
			if (c == '2' && cs < cnt_table - 1) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == '?')
			{
				show_help("tbirth.txt#Class");
			}

			if (c >= 'a' && c <= 'a' + cnt_table - 1)
			{
				k = c - 'a';
				if (!table[k].flag_fix)continue;

				//藍はHARD以上限定
				if (table[k].class_id == CLASS_RAN && difficulty < DIFFICULTY_HARD) continue;
				if (table[k].class_id == CLASS_YUKARI && difficulty < DIFFICULTY_HARD) continue;

				break;
			}
		}
		clear_from(10);
		if (!ok)
		{
			continue; //作品選択まで戻る
		}



		roff_to_buf(table[k].info, 74, temp, sizeof(temp));
		t = temp;
		for (i = 0; i< 12; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}

		if(table[k].race == 255 || table[k].class_id == 255)
		{
			c_put_str(TERM_RED, "ERROR:このプレーヤーの種族かクラスが設定されていない", 10, 10);
			inkey();
			return (FALSE);
		}
		if(!class_info[table[k].class_id].flag_only_unique)
		{
			c_put_str(TERM_RED, "ERROR:このクラスにユニーククラスフラグが設定されていない", 10, 10);
			inkey();
			return (FALSE);
		}

		/*:::名前変更*/
		strcpy(old_name, player_name);
		strcpy(player_name, table[k].name);
		put_str("名前  :　　　　　　　　　　　　　　　　", 1,16);
		c_put_str(TERM_L_BLUE, player_name, 1, 24);

		/*:::性別確定*/
		p_ptr->psex = table[k].sex;
		sp_ptr = &sex_info[p_ptr->psex];
		c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 15);

		/*:::種族確定*/
		p_ptr->prace = table[k].race;
		rp_ptr = &race_info[p_ptr->prace];
		c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);

		/*:::職業確定*/
		p_ptr->pclass = table[k].class_id;
		cp_ptr = &class_info[p_ptr->pclass];
		if(p_ptr->psex == SEX_MALE )strcpy(player_class_name, cp_ptr->title);
		else strcpy(player_class_name, cp_ptr->f_title);



		///del m_info削除
		//mp_ptr = &m_info[0];
		if(p_ptr->psex == SEX_MALE)	c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 15);
		else						c_put_str(TERM_L_BLUE, cp_ptr->f_title, 5, 15);



		mul_score = cp_ptr->score_mult * rp_ptr->score_mult / 100;

		extra_mode_score_mult(&mul_score,p_ptr->pclass); 

		mul_exp = cp_ptr->c_exp + rp_ptr->r_exp - 100;

				str = rp_ptr->title;
				c_put_str(TERM_L_BLUE, table[k].name, 3, 40);
				put_str("の能力値修正", 3, 40+strlen( table[k].name));
				put_str("腕力 知能 賢さ 器用 耐久 魅力 経験  スコア倍率", 4, 40);
				if(table[k].race == RACE_ANDROID)
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d  **** %4d%% ",
					/*::: 妖怪人形は経験値補正を表示しない*/
					rp_ptr->r_adj[0]+cp_ptr->c_adj[0], 
					rp_ptr->r_adj[1]+cp_ptr->c_adj[1],
					rp_ptr->r_adj[2]+cp_ptr->c_adj[2],
					rp_ptr->r_adj[3]+cp_ptr->c_adj[3],
					rp_ptr->r_adj[4]+cp_ptr->c_adj[4],
					rp_ptr->r_adj[5]+cp_ptr->c_adj[5],
					mul_score);

				else
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %4d%% ",
					rp_ptr->r_adj[0]+cp_ptr->c_adj[0], 
					rp_ptr->r_adj[1]+cp_ptr->c_adj[1],
					rp_ptr->r_adj[2]+cp_ptr->c_adj[2],
					rp_ptr->r_adj[3]+cp_ptr->c_adj[3],
					rp_ptr->r_adj[4]+cp_ptr->c_adj[4],
					rp_ptr->r_adj[5]+cp_ptr->c_adj[5],
					mul_exp, mul_score);
				c_put_str(TERM_L_BLUE, buf, 5, 40);

		if (!get_check_strict("よろしいですか？", CHECK_DEFAULT_Y))
		{
			Term_clear();
			put_str("キャラクターを作成します。('S'やり直す, 'Q'終了, '?'ヘルプ)", 8, 10);
			put_str("名前  :　　　　　　　　　　　　　　　　", 1,16);
			put_str("性別        :", 3, 1);
			put_str("種族        :", 4, 1);
			put_str("職業        :", 5, 1);
			strcpy(player_name, old_name);
			c_put_str(TERM_L_BLUE, player_name, 1, 24);
			continue; //作品選択まで戻る
		}
		break;
	}
	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);
	return (TRUE);

}



/*:::外来人設定用　種族職業固定*/
static bool get_outside_player(void)
{
	int i;

	cptr str;
	char c;

	char temp[80*16];
	cptr t;
	char    buf[80], cur[80];
	int mul_exp, mul_score;

	/*:::種族確定*/
	p_ptr->prace = RACE_HUMAN;
	rp_ptr = &race_info[p_ptr->prace];
	c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);

	/*:::職業確定*/
	p_ptr->pclass = CLASS_OUTSIDER;
	cp_ptr = &class_info[p_ptr->pclass];
	/*:::まだ性別を設定してないが職業名は男女共通*/
	strcpy(player_class_name, cp_ptr->title); 
	c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 15);

	clear_from(10);
	roff_to_buf(class_jouhou[p_ptr->pclass], 74, temp, sizeof(temp));
	t = temp;
	for (i = 0; i< 12; i++)
	{
		if(t[0] == 0)
			break; 
		else
		{
			prt(t, 12+i, 3);
			t += strlen(t) + 1;
		}
	}
	mul_score = cp_ptr->score_mult * rp_ptr->score_mult / 100;
	mul_exp = cp_ptr->c_exp + rp_ptr->r_exp - 100;

				str = cp_ptr->title;
				c_put_str(TERM_L_BLUE, str, 3, 40);
				put_str("の能力値修正", 3, 40+strlen( str));
				put_str("腕力 知能 賢さ 器用 耐久 魅力 経験  スコア倍率", 4, 40);
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %4d%% ",
					rp_ptr->r_adj[0]+cp_ptr->c_adj[0], 
					rp_ptr->r_adj[1]+cp_ptr->c_adj[1],
					rp_ptr->r_adj[2]+cp_ptr->c_adj[2],
					rp_ptr->r_adj[3]+cp_ptr->c_adj[3],
					rp_ptr->r_adj[4]+cp_ptr->c_adj[4],
					rp_ptr->r_adj[5]+cp_ptr->c_adj[5],
					mul_exp, mul_score);
				c_put_str(TERM_L_BLUE, buf, 5, 40);

	if (!get_check_strict("よろしいですか？", CHECK_DEFAULT_Y))	return (FALSE);

	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);
	clear_from(10);

	return (TRUE);

}

/*
 * Helper function for 'player_birth()'
 *
 * The delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.  And it cuts down on player stupidity.
 */
/*:::新キャラクター生成*/
///system class race seikaku 最重要
static bool player_birth_aux(void)
{
	int i, k, n, cs, os;

	int mode = 0;

	bool flag = FALSE;
	bool prev = FALSE;
	bool flag_unique = FALSE;
	bool flag_general = FALSE;
	bool flag_outsider = FALSE;


	cptr str;

	char c;

#if 0
	char p1 = '(';
#endif

	char p2 = ')';
	char b1 = '[';
	char b2 = ']';

	char buf[80], cur[80];


	/*** Intro ***/

	/* Clear screen */
	Term_clear();

	/* Title everything */
#ifdef JP
//	put_str("名前  :", 1,26);
	put_str("名前  :", 1,16);
#else
	put_str("Name  :", 1,26);
#endif

#ifdef JP
	put_str("性別        :", 3, 1);
#else
	put_str("Sex         :", 3, 1);
#endif

#ifdef JP
	put_str("種族        :", 4, 1);
#else
	put_str("Race        :", 4, 1);
#endif

#ifdef JP
	put_str("職業        :", 5, 1);
#else
	put_str("Class       :", 5, 1);
#endif


	/* Dump the default name */
//	c_put_str(TERM_L_BLUE, player_name, 1, 34);
	c_put_str(TERM_L_BLUE, player_name, 1, 24);


	/*** Instructions ***/

	/* Display some helpful information */
#ifdef JP
	put_str("キャラクターを作成します。('S'やり直す, 'Q'終了, '?'ヘルプ)", 8, 10);
#else
	put_str("Make your charactor. ('S' Restart, 'Q' Quit, '?' Help)", 8, 10);
#endif

	

	while(1)
	{
		char temp[80*9];
		cptr t;
		cptr select_difficulty[5] = {
			"a)EASY",
			"b)NORMAL",
			"c)HARD",
			"d)LUNATIC",
			"e)EXTRA",
		};

		put_str("難易度を選択してください：", 10, 10);
		cs = 0;
		while(1)
		{

		for(n=0;n<5;n++)
			{
				if(n == cs) c_put_str(TERM_YELLOW, select_difficulty[n], 12 + n, 10);
				//else if(n == 4 && n != cs) c_put_str(TERM_L_DARK, select_difficulty[n], 12 + n, 10);
				else put_str(select_difficulty[n], 12 + n, 10);
			}
			c = inkey();
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);
			if (c == ' ' || c == '\r' || c == '\n')
			{
				c = cs + 'a';
			}
			if (c == '2' && cs < 4) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == 'a')
			{
				difficulty = DIFFICULTY_EASY;	
				break;
			}
			if (c == 'b')
			{
				difficulty = DIFFICULTY_NORMAL;	
				break;
			}
			if (c == 'c')
			{
				difficulty = DIFFICULTY_HARD;	
				break;
			}
			if (c == 'd')
			{
				difficulty = DIFFICULTY_LUNATIC;	
				break;
			}
			if (c == 'e')
			{
				difficulty = DIFFICULTY_EXTRA;	
				break;
			}
			if (c == '?')
			{
				show_help("tbirth.txt#Difficulty");
			}

		}
		clear_from(10);
		roff_to_buf(info_difficulty[difficulty], 74, temp, sizeof(temp));
		t = temp;

		for (i = 0; i< 9; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}
		/*
		if( difficulty == DIFFICULTY_EXTRA)
		{
			char tester;
			prt("何かキーを押してください", 0, 0);
			tester = inkey();
			
			prt("", 0, 0);
			if(tester == '@') break;
		}
		else 
		*/
		if (get_check_strict("よろしいですか？", CHECK_DEFAULT_Y)) break;
		clear_from(10);
	}
	clear_from(10);

	/*:::難易度別にオプションを設定する。この変数はセーブ＆ロードされないので、dungeon.cのextract_option_vars()に同じ記述をする必要がある*/
	switch(difficulty)
	{
	case DIFFICULTY_EASY:
		easy_band = TRUE;
		smart_learn = FALSE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_NORMAL:
		easy_band = FALSE;
		smart_learn = FALSE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_HARD:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_LUNATIC:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = TRUE;
		ironman_nightmare = TRUE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_EXTRA:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = TRUE;
		break;
	}

	{
		cptr select_mode[3] = {
			"a)幻想郷で少しは名の知られた者だ。",
			"b)幻想郷の名もなき住人だ。",
			"c)幻想郷？何だそれは？",
		};

		put_str("あなたは・・・", 10, 10);
		cs = 0;
		while(1)
		{
			for(n=0;n<3;n++)
			{
				if(n == cs) c_put_str(TERM_YELLOW, select_mode[n], 12 + n, 10);
				else put_str(select_mode[n], 12 + n, 10);
			}
			c = inkey();
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);
			if (c == ' ' || c == '\r' || c == '\n')
			{
				c=cs+'a';
			}
			if (c == '2' && cs < 2) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == 'a')
			{
				flag_unique = TRUE;
				break;
			}
			if (c == 'b')
			{
				flag_general = TRUE;
				break;
			}
			if (c == 'c')
			{
				flag_outsider = TRUE;
				break;
			}
			if (c == '?')
			{
				show_help("tbirth.txt#Class");
			}
		}
	}
	clear_from(10);


	//外来人専用（種族職業固定）
	if ( flag_outsider && !get_outside_player()) return FALSE;

	/*:::ユニークプレイヤー（性別、名前、種族、特殊職業固定セット)*/
	if (flag_unique && !get_unique_player()) return FALSE;

	//性別設定
if(flag_general || flag_outsider)
{

	/*** Player sex ***/

	/* Extra info */
#ifdef JP
//	put_str("注意：《性別》の違いはゲーム上ほとんど影響を及ぼしません。", 23, 5);
	put_str("注意：《性別》の違いはゲーム上ほとんど影響を及ぼしませんが、装備品の多くは女性用です。", 23, 5);
#else
	put_str("Note: Your 'sex' does not have any significant gameplay effects.", 23, 5);
#endif


	/* Prompt for "Sex" */
	for (n = 0; n < MAX_SEXES; n++)
	{
		/* Analyze */
		sp_ptr = &sex_info[n];

		/* Display */
#ifdef JP
		sprintf(buf, "%c%c%s", I2A(n), p2, sp_ptr->title);
#else
		sprintf(buf, "%c%c %s", I2A(n), p2, sp_ptr->title);
#endif
		put_str(buf, 12 + (n/5), 2 + 15 * (n%5));
	}

#ifdef JP
	sprintf(cur, "%c%c%s", '*', p2, "ランダム");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Choose */
	k = -1;
	cs = 0;
	os = MAX_SEXES;
	while (1)
	{
		if (cs != os)
		{
			put_str(cur, 12 + (os/5), 2 + 15 * (os%5));
			if(cs == MAX_SEXES)
#ifdef JP
				sprintf(cur, "%c%c%s", '*', p2, "ランダム");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
			else
			{
				sp_ptr = &sex_info[cs];
				str = sp_ptr->title;
#ifdef JP
				sprintf(cur, "%c%c%s", I2A(cs), p2, str);
#else
				sprintf(cur, "%c%c %s", I2A(cs), p2, str);
#endif
			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/5), 2 + 15 * (cs%5));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "性別を選んで下さい (%c-%c) ('='初期オプション設定): ", I2A(0), I2A(n-1));
#else
		sprintf(buf, "Choose a sex (%c-%c) ('=' for options): ", I2A(0), I2A(n-1));
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == MAX_SEXES)
				k = randint0(MAX_SEXES);
			else
				k = cs;
			break;
		}
		if (c == '*')
		{
			k = randint0(MAX_SEXES);
			break;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < MAX_SEXES) cs++;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < MAX_SEXES))
		{
			cs = k;
			continue;
		}
		else k = -1;
		//if (c == '?') do_cmd_help();
		//else 
		if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
		}
		else if(c != '4' && c != '6')bell();
	}

	/* Set sex */
	p_ptr->psex = k;
	sp_ptr = &sex_info[p_ptr->psex];

	/* Display */
	c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 15);

	/* Clean up */
	clear_from(10);
	}
	//種族職業設定
	if(flag_general)
	{
	/* Choose the players race */
	p_ptr->prace = 0;
	while(1)
	{
		char temp[80*10];
		cptr t;

		if (!get_player_race()) return FALSE;

		clear_from(10);

		roff_to_buf(race_jouhou[p_ptr->prace], 74, temp, sizeof(temp));
		t = temp;

		for (i = 0; i< 10; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}
		if(rp_ptr->flag_nofixed)
		{
			prt("何かキーを押してください", 0, 0);
			(void)inkey();
			prt("", 0, 0);
		}
		else if (get_check_strict("よろしいですか？", CHECK_DEFAULT_Y)) break;

		clear_from(10);
		c_put_str(TERM_WHITE, "                          ", 4, 15);
	}

	/* Clean up */
	clear_from(10);
	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);




	/* Choose the players class */
	p_ptr->pclass = 0;
	while(1)
	{
		char temp[80*9];
		cptr t;

		if (!get_player_class()) return FALSE;

		clear_from(10);
		roff_to_buf(class_jouhou[p_ptr->pclass], 74, temp, sizeof(temp));
		t = temp;

		for (i = 0; i< 9; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}

		if(cp_ptr->flag_nofixed)
		{
			prt("何かキーを押してください", 0, 0);
			(void)inkey();
			prt("", 0, 0);
		}
		else if (get_check_strict("よろしいですか？", CHECK_DEFAULT_Y)) break;

		c_put_str(TERM_WHITE, "                 ", 5, 15);
	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);


	}

	///mod140209 普通の住人はここで職業名を変えられるようにする
	if(p_ptr->pclass == CLASS_CIVILIAN)
		{
			get_class_name();
			c_put_str(TERM_WHITE, "                     ", 5, 15);
			c_put_str(TERM_L_BLUE, player_class_name, 5, 15);
		}

}//一般職処理終了


	put_str("                                           ", 3, 40);
	put_str("                                           ", 4, 40);
	put_str("                                           ", 5, 40);
	put_str("                                           ", 6, 40);


	/* Choose the magic realms */
	if (!get_player_realms()) return FALSE;

	/* Choose the players seikaku */
	p_ptr->pseikaku = 0;
	while(1)
	{
		char temp[80*8];
		cptr t;

		if (!get_player_seikaku()) return FALSE;

		clear_from(10);
		if(p_ptr->pseikaku == SEIKAKU_SPECIAL)
			roff_to_buf(seikaku_special_jouhou[get_special_seikaku_index(p_ptr->pclass)], 74, temp, sizeof(temp));
		else
			roff_to_buf(seikaku_jouhou[p_ptr->pseikaku], 74, temp, sizeof(temp));

		t = temp;

		for (i = 0; i< 6; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}
		if(ap_ptr->flag_nofixed)
		{
			prt("何かキーを押してください", 0, 0);
			(void)inkey();
			prt("", 0, 0);
		}
		else if(old_data_cheat_flag && p_ptr->pseikaku == SEIKAKU_COLLECTOR)
		{
			prt("前回のセーブデータにチートフラグが立っているため選択不可:", 0, 0);
			(void)inkey();
			prt("", 0, 0);
		}
		else if(old_collector && p_ptr->pseikaku != SEIKAKU_COLLECTOR)
		{
			screen_save();
			Term_clear();
			prt("今回選択した性格は「蒐集家」ではないので、", 10, 20);
			prt("前回までに集めたエントランスのアイテムはリセットされます。", 11, 20);
			(void)inkey();
			screen_load();
			if (get_check_strict("よろしいですか？", CHECK_OKAY_CANCEL)) break;

		}
		else if (get_check_strict("よろしいですか？", CHECK_DEFAULT_Y)) break;
		put_str("                                                       ", 1, 24);
		c_put_str(TERM_L_BLUE, player_name, 1, 24);
		//c_put_str(TERM_L_BLUE, player_name, 1, 34);
		prt("", 1, 34+strlen(player_name));
	}

	//-Hack- 赤蛮奇専用性格のとき魔法領域を選択し直す
	if (is_special_seikaku(SEIKAKU_SPECIAL_SEKIBANKI))
	{
		if (!get_player_realms()) return FALSE;

	}

	//-Hack- 華扇専用性格のとき種族変更し魔法領域を選択し直す
	if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
	{
		p_ptr->prace = RACE_ONI;
		rp_ptr = &race_info[p_ptr->prace];
		put_str("種族        :　　　　　　　", 4, 1);
		c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);
		if (!get_player_realms()) return FALSE;
	}



	/* Clean up */
	clear_from(10);
	put_str("                                                       ", 3, 40);
	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);

	screen_save();
#ifdef JP
	do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
	do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

	screen_load();


#if 0
#ifdef ALLOW_AUTOROLLER

	/*** Autoroll ***/

	if (autoroller || autochara)
	{
		/* Clear fields */
		auto_round = 0L;
	}

	/* Initialize */
	if (autoroller)
	{
		if (!get_stat_limits()) return FALSE;
	}

	if (autochara)
	{
		///tmp131221 身長体重年齢関連の処理を非表示に
		//if (!get_chara_limits()) return FALSE;
	}

#endif /* ALLOW_AUTOROLLER */

	/* Clear */
	clear_from(10);

	/* Reset turn; before auto-roll and after choosing race */
	init_turn();

	/*** Generate ***/

	/* Roll */
	while (TRUE)
	{
		int col;

		col = 42;

		if (autoroller || autochara)
		{
			Term_clear();

			/* Label count */
#ifdef JP
			put_str("回数 :", 10, col+13);
#else
			put_str("Round:", 10, col+13);
#endif


			/* Indicate the state */
#ifdef JP
			put_str("(ESCで停止)", 12, col+13);
#else
			put_str("(Hit ESC to stop)", 12, col+13);
#endif
		}

		/* Otherwise just get a character */
		else
		{
			/* Get a new character */
			get_stats();

			/* Roll for age/height/weight */
			get_ahw();

			/* Roll for social class */
			get_history();
		}

		/* Feedback */
		if (autoroller)
		{
			/* Label */
#ifdef JP
			put_str("最小値", 2, col+5);
#else
			put_str(" Limit", 2, col+5);
#endif


			/* Label */
#ifdef JP
			put_str("成功率", 2, col+13);
#else
			put_str("  Freq", 2, col+13);
#endif


			/* Label */
#ifdef JP
			put_str("現在値", 2, col+24);
#else
			put_str("  Roll", 2, col+24);
#endif


			/* Put the minimal stats */
			for (i = 0; i < 6; i++)
			{
				int j, m;

				/* Label stats */
				put_str(stat_names[i], 3+i, col);

				/* Race/Class bonus */
				j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i];

				/* Obtain the current stat */
				m = adjust_stat(stat_limit[i], j);

				/* Put the stat */
				cnv_stat(m, buf);
				c_put_str(TERM_L_BLUE, buf, 3+i, col+5);
			}
		}

		/* Auto-roll */
		while (autoroller || autochara)
		{
			bool accept = TRUE;

			/* Get a new character */
			get_stats();

			/* Advance the round */
			auto_round++;

			/* Hack -- Prevent overflow */
			if (auto_round >= 1000000000L)
			{
				auto_round = 1;

				if (autoroller)
				{
					for (i = 0; i < 6; i++)
					{
						stat_match[i] = 0;
					}
				}
			}

			if (autoroller)
			{
				/* Check and count acceptable stats */
				for (i = 0; i < 6; i++)
				{
					/* This stat is okay */
					if (p_ptr->stat_max[i] >= stat_limit[i])
					{
						stat_match[i]++;
					}

					/* This stat is not okay */
					else
					{
						accept = FALSE;
					}
				}
			}

			/* Break if "happy" */
			if (accept)
			{
				/* Roll for age/height/weight */
				get_ahw();

				/* Roll for social class */
				get_history();
				///del131221 体格オートロール無効
				/*
				if (autochara)
				{
					if ((p_ptr->age < chara_limit.agemin) || (p_ptr->age > chara_limit.agemax)) accept = FALSE;
					if ((p_ptr->ht < chara_limit.htmin) || (p_ptr->ht > chara_limit.htmax)) accept = FALSE;
					if ((p_ptr->wt < chara_limit.wtmin) || (p_ptr->wt > chara_limit.wtmax)) accept = FALSE;
					if ((p_ptr->sc < chara_limit.scmin) || (p_ptr->sc > chara_limit.scmax)) accept = FALSE;
				}
				*/
				if (accept) break;
			}

			/* Take note every x rolls */
			flag = (!(auto_round % AUTOROLLER_STEP));

			/* Update display occasionally */
			if (flag)
			{
				/* Dump data */
				birth_put_stats();

				/* Dump round */
				put_str(format("%10ld", auto_round), 10, col+20);

#ifdef AUTOROLLER_DELAY
				/* Delay 1/10 second */
				if (flag) Term_xtra(TERM_XTRA_DELAY, 10);
#endif

				/* Make sure they see everything */
				Term_fresh();

				/* Do not wait for a key */
				inkey_scan = TRUE;

				/* Check for a keypress */
				if (inkey())
				{
					/* Roll for age/height/weight */
					get_ahw();

					/* Roll for social class */
					///sysdel
					get_history();

					break;
				}
			}
		}

		if (autoroller || autochara) sound(SOUND_LEVEL);

		/* Flush input */
		flush();


		/*** Display ***/

		/* Mode */
		mode = 0;

		/* Roll for base hitpoints */
		get_extra(TRUE);

		/* Roll for gold */
		get_money();

		/* Hack -- get a chaos patron even if you are not a chaos warrior */
		p_ptr->chaos_patron = (s16b)randint0(MAX_PATRON);

		/* Input loop */
		while (TRUE)
		{
			/* Calculate the bonuses and hitpoints */
			p_ptr->update |= (PU_BONUS | PU_HP);

			/* Update stuff */
			update_stuff();

			/* Fully healed */
			p_ptr->chp = p_ptr->mhp;

			/* Fully rested */
			p_ptr->csp = p_ptr->msp;

			/* Display the player */
			display_player(mode);

			/* Prepare a prompt (must squeeze everything in) */
			Term_gotoxy(2, 23);
			Term_addch(TERM_WHITE, b1);
#ifdef JP
			Term_addstr(-1, TERM_WHITE, "'r' 次の数値");
#else
			Term_addstr(-1, TERM_WHITE, "'r'eroll");
#endif

#ifdef JP
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p' 前の数値");
#else
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p'previous");
#endif

#ifdef JP
			if (mode) Term_addstr(-1, TERM_WHITE, ", 'h' その他の情報");
#else
			if (mode) Term_addstr(-1, TERM_WHITE, ", 'h' Misc.");
#endif

#ifdef JP
			else Term_addstr(-1, TERM_WHITE, ", 'h' 生い立ちを表示");
#else
			else Term_addstr(-1, TERM_WHITE, ", 'h'istory");
#endif

#ifdef JP
			Term_addstr(-1, TERM_WHITE, ", Enter この数値に決定");
#else
			Term_addstr(-1, TERM_WHITE, ", or Enter to accept");
#endif

			Term_addch(TERM_WHITE, b2);

			/* Prompt and get a command */
			c = inkey();

			/* Quit */
			if (c == 'Q') birth_quit();

			/* Start over */
			if (c == 'S') return (FALSE);

			/* Escape accepts the roll */
			if (c == '\r' || c == '\n' || c == ESCAPE) break;

			/* Reroll this character */
			if ((c == ' ') || (c == 'r')) break;

			/* Previous character */
			if (prev && (c == 'p'))
			{
				load_prev_data(TRUE);
				continue;
			}

			/* Toggle the display */
			if ((c == 'H') || (c == 'h'))
			{
				mode = ((mode != 0) ? 0 : 1);
				continue;
			}

			/* Help */
			if (c == '?')
			{
#ifdef JP
				show_help("jbirth.txt#AutoRoller");
#else
				show_help("birth.txt#AutoRoller");
#endif
				continue;
			}
			else if (c == '=')
			{
				screen_save();
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
				do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

				screen_load();
				continue;
			}

			/* Warning */
#ifdef JP
			bell();
#else
			bell();
#endif

		}

		/* Are we done? */
		if (c == '\r' || c == '\n' || c == ESCAPE) break;

		/* Save this for the "previous" character */
		save_prev_data(&previous_char);
		previous_char.quick_ok = FALSE;

		/* Note that a previous roll exists */
		prev = TRUE;
	}
#endif
	///mod140510 ボーナスポイント制にした
	while(1)
	{
		bool ok = FALSE;
		get_bonus_points();
		init_turn();
		get_ahw();
		get_history();
		get_extra(TRUE);
		get_money();
		//野良神様特殊処理　最初の神格タイプを決定しカオスパトロン変数へ格納する deity_table[]の3～14
		//カオスパトロン変数を使うのはクイックスタート時に保持するため
		if(p_ptr->prace == RACE_STRAYGOD)
		{
			if(p_ptr->pclass == CLASS_SHIZUHA) p_ptr->chaos_patron = 2; 
			else if(p_ptr->pclass == CLASS_MINORIKO) p_ptr->chaos_patron = 1;
			else if (p_ptr->pclass == CLASS_EIKA) p_ptr->chaos_patron = 79;
			else p_ptr->chaos_patron = 2 + (s16b)randint1(12);

			p_ptr->race_multipur_val[3] = p_ptr->chaos_patron;
		}
		else
			p_ptr->chaos_patron = (s16b)randint0(MAX_PATRON);

		//v1.1.70 種族「動物霊」の所属勢力を決める
		if (p_ptr->prace == RACE_ANIMAL_GHOST)
		{
			get_animal_ghost_family(TRUE);
		}


		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA);
		update_stuff();
		p_ptr->chp = p_ptr->mhp;
		p_ptr->csp = p_ptr->msp;
		display_player(0);
		Term_gotoxy(2, 23);
		Term_addstr(-1, TERM_WHITE, "'r' ボーナスポイントを再ロール");
		Term_addstr(-1, TERM_WHITE, ", Enter この数値に決定");
		while(1)
		{
			c = inkey();
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);
			if (c == '\r' || c == '\n' || c == ESCAPE)
			{
				ok = TRUE;
				break;
			}
			if ((c == 'r')) break;
		}
		if(ok) break;
		put_str("                                                       ", 3, 30);
		put_str("                                                       ", 4, 30);
		put_str("                                                       ", 5, 30);
		put_str("                                                       ", 6, 30);
		put_str("                                                       ", 7, 30);
		put_str("                                                       ", 8, 30);

	}
	///mod150315 妖精特殊能力決定(最初からキャラメイクした時用)
	if(p_ptr->prace == RACE_FAIRY) get_fairy_type(TRUE);



	/* Clear prompt */
	clear_from(23);

	/* Get a name, recolor it, prepare savefile */

	if(!flag_unique) get_name();


	/* Process the player name */
	process_player_name(creating_savefile);

	/*** Edit character background ***/
	edit_history();

	/*** Finish up ***/

	get_max_stats();


	///del140105 virtue
	//get_virtues();

	/* Prompt for it */
#ifdef JP
	prt("[ 'Q' 中断, 'S' 初めから, Enter ゲーム開始 ]", 23, 14);
#else
	prt("['Q'uit, 'S'tart over, or Enter to continue]", 23, 10);
#endif


	/* Get a key */
	c = inkey();

	/* Quit */
	if (c == 'Q') birth_quit();

	/* Start over */
	if (c == 'S') return (FALSE);


	/* Initialize random quests */
	init_dungeon_quests();

	/* Save character data for quick start */
	save_prev_data(&previous_char);
	previous_char.quick_ok = TRUE;

	/* Accept */
	return (TRUE);
}


/*
 * Ask whether the player use Quick Start or not.
 */
/*:::クイックスタートの確認　続行ならデータロード　クイックスタートしないならFALSEを返す*/
static bool ask_quick_start(void)
{
	/* Doesn't have previous data */
	if (!previous_char.quick_ok) return FALSE;


	/* Clear screen */
	Term_clear();

	/* Extra info */
#ifdef JP
	put_str("クイック・スタートを使うと以前と全く同じキャラクターで始められます。", 11, 5);
#else
	put_str("Do you want to use the quick start function(same character as your last one).", 11, 2);
#endif

	/* Choose */
	while (1)
	{
		char c;

#ifdef JP
		put_str("クイック・スタートを使いますか？[y/N]", 14, 10);
#else
		put_str("Use quick start? [y/N]", 14, 10);
#endif
		c = inkey();

		if (c == 'Q') quit(NULL);
		else if (c == 'S') return (FALSE);
		else if (c == '?')
		{
#ifdef JP
			show_help("tbirth.txt#QuickStart");
#else
			show_help("birth.txt#QuickStart");
#endif
		}
		else if ((c == 'y') || (c == 'Y'))
		{
			/* Yes */
			break;
		}
		else
		{
			/* No */
			return FALSE;
		}
	}

	/*:::クイックスタート用に色々なデータを読み込む*/
	load_prev_data(FALSE);

	//v1.1.30 init_dungeon_quests()とinit_turn()を逆にする。こうしないと大幅に日数経過したセーブデータでクイックスタートしたときにランクエモンスターに強敵が出現するらしい
	/*:::種族ごとにターン計算*/
	init_turn();
	/*:::クエスト初期化*/
	init_dungeon_quests();

	sp_ptr = &sex_info[p_ptr->psex];
	rp_ptr = &race_info[p_ptr->prace];
	cp_ptr = &class_info[p_ptr->pclass];
	//mp_ptr = &m_info[p_ptr->pclass];
	//v1.1.32 特殊性格実装
	//ap_ptr = &seikaku_info[p_ptr->pseikaku];
	ap_ptr = get_ap_ptr(p_ptr->prace,p_ptr->pclass,p_ptr->pseikaku);

	/* Calc hitdie, but don't roll */
	get_extra(FALSE);

	/* Calculate the bonuses and hitpoints */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Update stuff */
	update_stuff();

	/* Fully healed */
	p_ptr->chp = p_ptr->mhp;

	/* Fully rested */
	p_ptr->csp = p_ptr->msp;

	/* Process the player name */
	process_player_name(FALSE);

	return TRUE;
}



/*:::クラス固定の永久変異を得る処理。ゲーム開始時と種族変容のとき使われる。*/
// 新たに得た生来変異が現在持っている変異と相反するときその変異を消さないといけない。　
// 面倒だから種族変容したら一度全部の変異が消えるようにしてしまおうか？
// ↑とりあえず種族変異の時は変異が全部消えることにした。思い出せないが相反変異が重複しないことを前提にコード書いた部分がどっかにあった気がする。
void gain_perma_mutation(void)
{
	p_ptr->muta1_perma = 0;
	p_ptr->muta2_perma = 0;
	p_ptr->muta3_perma &= MUT3_GOOD_LUCK ; //Hack 魔法店報酬で得た白いオーラは転生とかしても消えない
	p_ptr->muta4_perma = 0;


	if(p_ptr->pseikaku == SEIKAKU_LUCKY) p_ptr->muta3_perma |= MUT3_GOOD_LUCK;
	if (p_ptr->prace == RACE_ONI)
	{
		if(p_ptr->pclass == CLASS_ZANMU)
			p_ptr->muta2_perma |= MUT2_HORNS;
		else
			p_ptr->muta2_perma |= MUT2_BIGHORN;
	}

	if(p_ptr->prace == RACE_NINGYO) p_ptr->muta3_perma |= MUT3_FISH_TAIL;
	if(p_ptr->pclass == CLASS_TEWI) p_ptr->muta3_perma |= MUT3_GOOD_LUCK;
	if(p_ptr->pclass == CLASS_SUIKA) p_ptr->muta2_perma |= MUT2_ALCOHOL;
	if(p_ptr->pclass == CLASS_KOISHI) p_ptr->muta2_perma |= (MUT2_RTELEPORT | MUT2_BIRDBRAIN);
	if(p_ptr->pclass == CLASS_YOSHIKA) p_ptr->muta3_perma |= MUT3_MORONIC;
	if(p_ptr->pclass == CLASS_PATCHOULI && p_ptr->magic_num2[0] != 1) p_ptr->muta2_perma |= MUT2_ASTHMA;
	if(p_ptr->pclass == CLASS_YUGI) p_ptr->muta3_perma |= MUT3_HYPER_STR;
	if (p_ptr->pclass == CLASS_EIKA) p_ptr->muta3_perma |= MUT3_PUNY;

	if(p_ptr->pclass == CLASS_KYOUKO) p_ptr->muta1_perma |= MUT1_BEAST_EAR;
	if(p_ptr->pclass == CLASS_KEINE) p_ptr->muta2_perma |= MUT2_HARDHEAD;
	if(p_ptr->pclass == CLASS_REMY) p_ptr->muta3_perma |= MUT3_GOOD_LUCK;
	if(p_ptr->pclass == CLASS_UTSUHO) p_ptr->muta2_perma |= (MUT2_BIRDBRAIN);
	if(p_ptr->pclass == CLASS_LUNAR) p_ptr->muta2_perma |= (MUT2_DISARM);
	if(p_ptr->pclass == CLASS_3_FAIRIES && !is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES)) p_ptr->muta2_perma |= (MUT2_DISARM);
	if(p_ptr->pclass == CLASS_YUKARI) p_ptr->muta3_perma |= MUT3_HYPER_INT;
	if(p_ptr->pclass == CLASS_EIRIN) p_ptr->muta3_perma |= MUT3_HYPER_INT;
	if(p_ptr->pclass == CLASS_TOYOHIME) p_ptr->muta3_perma |= MUT3_GOOD_LUCK;
	if(p_ptr->pclass == CLASS_AUNN) p_ptr->muta2_perma |= MUT2_BIGHORN;

	if (p_ptr->pclass == CLASS_SHION) p_ptr->muta3_perma |= MUT3_BAD_LUCK;

	if (p_ptr->pclass == CLASS_KUTAKA) p_ptr->muta2_perma |= (MUT2_BIRDBRAIN);

	if(p_ptr->pclass == CLASS_URUMI) p_ptr->muta2_perma |= MUT2_BIGHORN;
	if (p_ptr->pclass == CLASS_BITEN) p_ptr->muta2_perma |= MUT2_TAIL;

	if (p_ptr->pclass == CLASS_SAKI)p_ptr->muta3_perma |= MUT3_SPEEDSTER | MUT3_WINGS;

	if (p_ptr->pclass == CLASS_YACHIE)	p_ptr->muta2_perma |= MUT2_BIGHORN | MUT2_BIGTAIL;

	if (p_ptr->pclass == CLASS_MOMOYO) p_ptr->muta1_perma |= MUT1_EAT_ROCK;

	if (p_ptr->pclass == CLASS_CHIYARI)	p_ptr->muta2_perma |= MUT2_BIGHORN | MUT2_BIGTAIL;

	if (p_ptr->pclass == CLASS_YUMA)
	{
		p_ptr->muta1_perma |= (MUT1_EAT_ROCK | MUT1_EAT_MAGIC);
		p_ptr->muta2_perma |= MUT2_BIGHORN;
	}


	p_ptr->muta1 = p_ptr->muta1_perma;
	p_ptr->muta2 = p_ptr->muta2_perma;
	p_ptr->muta3 = p_ptr->muta3_perma;

	p_ptr->muta4 = p_ptr->muta4_perma;


}

/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
void player_birth(void)
{
	int i, j;
	char buf[80];

	playtime = 0;

	//性格コレクター用のフラグ　こんなフラグ作らなくても既存のどれかのフラグでどうにかなるのかもしれないが調べてない
	if(p_ptr->noscore) old_data_cheat_flag = TRUE;
	if(p_ptr->pseikaku == SEIKAKU_COLLECTOR) old_collector = TRUE;

	/* 
	 * Wipe monsters in old dungeon
	 * This wipe destroys value of m_list[].cur_num .
	 */
	wipe_m_list();

	/* Wipe the player */
	player_wipe();

	/* Create a new character */

	/* Quick start? */
	/*:::クイックスタートの確認。Yなら前回データをロードし、そうでないならplayer_birth_aux()をループ*/
	if (!ask_quick_start())
	{
		//v1.1.58
		init_music_hack();
		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_NEW_GAME);

		/* No, normal start */
		while (1)
		{
			/* Roll up a new character */
			if (player_birth_aux()) break;

			/* Wipe the player */
			player_wipe();
		}
	}

	//鬼の角やラッキーマンのオーラなど生来型変異を得る
	gain_perma_mutation();

	//v2.0.14 ここに一部の職業の初期設定処理があったがprocess_player()のhack_startupのところにまとめた


	/* Note player birth in the message recall */
	message_add(" ");
	message_add("  ");
	message_add("====================");
	message_add(" ");
	message_add("  ");

	///system ユニークプレイヤー実装時ここの記述変えよう
#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- 新規ゲーム開始 --------");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- Start New Game --------");
#endif
	do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

#ifdef JP
	sprintf(buf,"                            性別に%sを選択した。", sex_info[p_ptr->psex].title);
#else
	sprintf(buf,"                            choose %s personality.", sex_info[p_ptr->psex].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            種族に%sを選択した。", race_info[p_ptr->prace].title);
#else
	sprintf(buf,"                            choose %s race.", race_info[p_ptr->prace].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            職業に%sを選択した。", class_info[p_ptr->pclass].title);
#else
	sprintf(buf,"                            choose %s class.", class_info[p_ptr->pclass].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	if (p_ptr->realm1)
	{
#ifdef JP
		sprintf(buf,"                            魔法の領域に%s%sを選択した。",realm_names[p_ptr->realm1], p_ptr->realm2 ? format("と%s",realm_names[p_ptr->realm2]) : "");
#else
		sprintf(buf,"                            choose %s%s realm.",realm_names[p_ptr->realm1], p_ptr->realm2 ? format(" realm and %s",realm_names[p_ptr->realm2]) : "");
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);
	}

#ifdef JP
//	sprintf(buf,"                            性格に%sを選択した。", seikaku_info[p_ptr->pseikaku].title);
	if(p_ptr->psex == SEX_MALE)
		sprintf(buf,"                            性格に%sを選択した。", ap_ptr->title);
	else
		sprintf(buf,"                            性格に%sを選択した。", ap_ptr->f_title);
#else
	sprintf(buf,"                            choose %s.", seikaku_info[p_ptr->pseikaku].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	/* Init the shops */
	for (i = 1; i < max_towns; i++)
	{
		for (j = 0; j < MAX_STORES; j++)
		{
			//性格「蒐集家」は、前回のセーブデータの博物館をクリアしない
			if(p_ptr->pseikaku == SEIKAKU_COLLECTOR && j == STORE_MUSEUM)
				continue;

			/* Initialize */
			store_init(i, j);
		}
	}

	/* Generate the random seeds for the wilderness */
	seed_wilderness();

	/* Give beastman a mutation at character birth */
	///sysdel race
	//if (p_ptr->prace == RACE_BEASTMAN) hack_startup = TRUE;
	//else 
	hack_startup = TRUE; //hack_mutationを全ての＠の開始時処理フラグとして作りなおした

	//v1.1.25 開始時バージョンを記録する
	start_ver[0] = H_VER_MAJOR;
	start_ver[1] = H_VER_MINOR;
	start_ver[2] = H_VER_PATCH;
	start_ver[3] = H_VER_EXTRA;

	/* Set the message window flag as default */
	if (!window_flag[1])
		window_flag[1] |= PW_MESSAGE;

	/* Set the inv/equip window flag as default */
	if (!window_flag[2])
		window_flag[2] |= PW_INVEN;
}

/*:::種族、職業、性格、領域の説明文をファイルにコピーする*/
void dump_yourself(FILE *fff)
{
	char temp[80*10];
	int i;
	cptr t;

	if (!fff) return;

	roff_to_buf(race_jouhou[p_ptr->prace], 78, temp, sizeof(temp));
	fprintf(fff, "\n\n");
#ifdef JP
	fprintf(fff, "種族: %s(%s)\n", race_info[p_ptr->prace].title, race_info[p_ptr->prace].E_title);
#else
	fprintf(fff, "Race: %s\n", race_info[p_ptr->prace].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}

	///mod150315 妖精特技
	if(p_ptr->prace == RACE_FAIRY && CHECK_FAIRY_TYPE)
		fprintf(fff, "(%s)\n",fairy_table[CHECK_FAIRY_TYPE].fairy_desc);


	///mod140206 ユニークプレーヤーはclass_jouhou[]に説明文がないので少しいじった
	if(cp_ptr->flag_only_unique)
	{
		for(i=0;i<UNIQUE_PLAYER_NUM;i++)
		{
			if(unique_player_table[i].class_id == p_ptr->pclass) break;
		}
		if(i==UNIQUE_PLAYER_NUM)
		{
			msg_print("ERROR:あなたのクラスIDがunique_player_table[]に登録されてない");
			return;
		}
		roff_to_buf(unique_player_table[i].info, 78, temp, sizeof(temp));		
	}
	else 
		roff_to_buf(class_jouhou[p_ptr->pclass], 78, temp, sizeof(temp));

	fprintf(fff, "\n");
#ifdef JP
	if(p_ptr->psex == SEX_MALE) fprintf(fff, "職業: %s(%s)\n", class_info[p_ptr->pclass].title,class_info[p_ptr->pclass].E_title);
	else fprintf(fff, "職業: %s(%s)\n", class_info[p_ptr->pclass].f_title,class_info[p_ptr->pclass].E_title);
#else
	fprintf(fff, "Class: %s\n", class_info[p_ptr->pclass].title);
#endif
	t = temp;

	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	if(p_ptr->pseikaku == SEIKAKU_SPECIAL)
		roff_to_buf(seikaku_special_jouhou[get_special_seikaku_index(p_ptr->pclass)], 74, temp, sizeof(temp));
	else
		roff_to_buf(seikaku_jouhou[p_ptr->pseikaku], 78, temp, sizeof(temp));

	fprintf(fff, "\n");
#ifdef JP
		//if(p_ptr->psex == SEX_MALE) fprintf(fff, "性格: %s\n", seikaku_info[p_ptr->pseikaku].title);
		//else fprintf(fff, "性格: %s\n", seikaku_info[p_ptr->pseikaku].f_title);
		if(p_ptr->psex == SEX_MALE) fprintf(fff, "性格: %s\n", ap_ptr->title);
		else fprintf(fff, "性格: %s\n", ap_ptr->f_title);
#else
	fprintf(fff, "Pesonality: %s\n", seikaku_info[p_ptr->pseikaku].title);
#endif
	t = temp;
	for (i = 0; i < 6; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	fprintf(fff, "\n");
	if (p_ptr->realm1)
	{
		roff_to_buf(realm_jouhou[p_ptr->realm1], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "魔法: %s\n", realm_names[p_ptr->realm1]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[p_ptr->realm1]);
#endif
		t = temp;
		for (i = 0; i < 6; i++)
		{
			if(t[0] == 0)
				break; 
			fprintf(fff, "%s\n",t);
			t += strlen(t) + 1;
		}
	}
	fprintf(fff, "\n");
	if (p_ptr->realm2)
	{
		roff_to_buf(realm_jouhou[p_ptr->realm2], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "魔法: %s\n", realm_names[p_ptr->realm2]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[p_ptr->realm2]);
#endif
		t = temp;
		for (i = 0; i < 6; i++)
		{
			if(t[0] == 0)
				break; 
			fprintf(fff, "%s\n",t);
			t += strlen(t) + 1;
		}
	}
}


//v1.1.32 特殊性格実装関連
//性格がSEIKAKU_SPECIALのとき、キャラに応じた性格infoをspecial_seikaku_info[]から持ってくる.
//それ以外の性格のときは普通のap_ptrを返す
const player_seikaku * get_ap_ptr(int race_idx, int class_idx, int seikaku_idx)
{

	if(seikaku_idx == SEIKAKU_SPECIAL)
	{
		int idx2 = get_special_seikaku_index(class_idx);
		if(idx2 >= MAX_SEIKAKU_SPECIAL)
		{
			msg_format("ERROR:特殊性格idxがMAX超過");

		}
		else
			return (const player_seikaku *) &seikaku_info_special[idx2];
	}
	
	return (const player_seikaku *) &seikaku_info[seikaku_idx];
}


//現在の＠が特殊性格idxに一致しているか判定する。
//キャラメイクやロード中のpseikakuやpclassが確定していない状態で使わないよう注意
bool is_special_seikaku(int idx)
{
	if (p_ptr->pseikaku != SEIKAKU_SPECIAL)
		return FALSE;

	if (get_special_seikaku_index(p_ptr->pclass) == idx)
		return TRUE;

	return FALSE;

}
