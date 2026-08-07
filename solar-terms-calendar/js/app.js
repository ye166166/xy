(function() {
  'use strict';

  const Z_EMOJI = { '鼠':'🐭','牛':'🐮','虎':'🐯','兔':'🐰','龙':'🐲','蛇':'🐍','马':'🐴','羊':'🐑','猴':'🐵','鸡':'🐔','狗':'🐶','猪':'🐷' };
  const DZ_EMOJI = { '子':'🐭','丑':'🐮','寅':'🐯','卯':'🐰','辰':'🐲','巳':'🐍','午':'🐴','未':'🐑','申':'🐵','酉':'🐔','戌':'🐶','亥':'🐷' };
  const S_EMOJI = { '春':'🌸','夏':'☀️','秋':'🍂','冬':'❄️' };
  const S_COLOR = { '春':'spring','夏':'summer','秋':'autumn','冬':'winter' };
  const F_EMOJI = { '春节':'🧧','元宵节':'🏮','龙抬头':'🐲','端午节':'🐉','七夕节':'💕','中元节':'🕯️','中秋节':'🌕','重阳节':'🌺','腊八节':'🥣','除夕':'🎆' };

  let year, month, currentTab = 'tabCalendar';

  function init() {
    const d = new Date();
    year = d.getFullYear();
    month = d.getMonth() + 1;
    GAME.init();
    if (document.readyState === 'loading') {
      document.addEventListener('DOMContentLoaded', boot);
    } else {
      boot();
    }
  }

  function boot() {
    try {
      refreshYear();
      refreshSeasonTabs();
      refreshShichen();
      renderCalendar();
      bind();
      switchTab('tabCalendar');
      setInterval(refreshShichen, 30000);
    } catch (e) {
      console.error(e);
      const g = document.getElementById('dateGrid');
      if (g) g.innerHTML = '<div style="text-align:center;padding:40px;color:#888;">加载失败，请<a href="javascript:location.reload()" style="color:#C41E1E;">刷新</a></div>';
    }
  }

  // ========== 年份卡片 ==========
  function refreshYear() {
    const info = getYearTianGanDiZhi(year);
    document.getElementById('yearEmoji').textContent = Z_EMOJI[info.shengXiao] || '📅';
    document.getElementById('yearName').textContent = info.fullName;
    document.getElementById('yearTags').innerHTML = [
      '天干：'+info.tianGan, '地支：'+info.diZhi,
      '五行：'+info.wuXing, '生肖：'+info.shengXiao, '阴阳：'+info.yinYang
    ].map(s => `<span class="yc-tag">${s}</span>`).join('');
  }

  // ========== 四季Tabs ==========
  function refreshSeasonTabs() {
    const s = getSeasonForMonth(month);
    document.querySelectorAll('.season-tab').forEach(t => {
      t.classList.toggle('active', t.dataset.season === s.name);
    });
  }

  // ========== 时辰 ==========
  function refreshShichen() {
    const sc = getCurrentShiChen();
    document.getElementById('srIcon').textContent = DZ_EMOJI[sc.diZhi] || '🕐';
    document.getElementById('srName').textContent = sc.name;
    document.getElementById('srDetail').textContent = sc.range + ' · 属' + sc.diZhi + ' · ' + sc.animal;
  }

  // ========== 渲染日历 ==========
  function renderCalendar() {
    const grid = document.getElementById('dateGrid');
    const info = getYearTianGanDiZhi(year);
    const minfo = getMonthTianGanDiZhi(year, month);
    const dzMap = ['子','丑','寅','卯','辰','巳','午','未','申','酉','戌','亥'];

    document.getElementById('mYear').textContent = year + '年';
    document.getElementById('mMonth').textContent = month + '月';
    document.getElementById('mGanzhi').textContent = info.fullName + ' · ' + minfo.tianGan + dzMap[month % 12] + '月';

    const firstDay = new Date(year, month - 1, 1);
    const lastDay = new Date(year, month, 0);
    const totalDays = lastDay.getDate();
    const startDow = firstDay.getDay();
    const terms = getSolarTermsForMonth(month);

    let html = '';
    for (let i = 0; i < startDow; i++) html += '<div class="date-cell empty"></div>';

    const td = new Date();
    const tdInMonth = td.getFullYear() === year && td.getMonth() === month - 1;

    for (let d = 1; d <= totalDays; d++) {
      const dt = new Date(year, month - 1, d);
      const dow = dt.getDay();
      let cls = 'date-cell';
      let label = '';
      let dots = '';

      if (dow === 0 || dow === 6) cls += ' weekend';

      const st = terms.find(t => t.day === d);
      const fests = getFestivalsForDate(year, month, d);

      if (st) { cls += ' solar-term'; label = st.name; dots += '<span class="dc-dot '+S_COLOR[st.season]+'"></span>'; }
      if (fests.length > 0) {
        if (!st) cls += ' festival';
        else cls += ' festival';
        if (label) label += '·'+fests[0].name;
        else label = fests[0].name;
        dots += '<span class="dc-dot festival"></span>';
      }
      if (tdInMonth && d === td.getDate()) cls += ' today';

      html += '<div class="'+cls+'" data-date="'+year+'-'+String(month).padStart(2,'0')+'-'+String(d).padStart(2,'0')+'">'+
        '<span class="dc-num">'+d+'</span>'+
        (label ? '<span class="dc-tag">'+label+'</span>' : '')+
        (dots ? '<div class="dc-dots">'+dots+'</div>' : '')+
        '</div>';
    }

    grid.innerHTML = html;
  }

  function changeMonth(delta) {
    month += delta;
    if (month < 1) { month = 12; year--; }
    if (month > 12) { month = 1; year++; }
    refreshAll();
  }

  function goToday() {
    const d = new Date();
    year = d.getFullYear();
    month = d.getMonth() + 1;
    refreshAll();
    window.scrollTo({ top: 0, behavior: 'smooth' });
  }

  function refreshAll() {
    refreshYear();
    refreshSeasonTabs();
    refreshShichen();
    renderCalendar();
  }

  // ========== Tab切换 ==========
  function switchTab(tabId) {
    currentTab = tabId;
    document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
    const page = document.getElementById(tabId);
    if (page) page.classList.add('active');

    document.querySelectorAll('.tab-item').forEach(t => {
      t.classList.toggle('active', t.dataset.tab === tabId);
    });

    const titles = { tabCalendar:'二十四节气日历', tabTerms:'二十四节气', tabFestivals:'传统节日', tabShichen:'十二时辰', tabSeasons:'四季调养', tabGame:'🎮 互动游戏' };
    document.getElementById('navTitle').textContent = titles[tabId] || '二十四节气日历';

    if (tabId === 'tabTerms') renderTermsList();
    if (tabId === 'tabFestivals') renderFestList();
    if (tabId === 'tabShichen') renderShichenList();
    if (tabId === 'tabSeasons') renderSeasonList();
    if (tabId === 'tabGame') { GAME.renderHub(); }

    window.scrollTo({ top: 0, behavior: 'smooth' });
  }

  // ========== 渲染各个列表页 ==========
  function renderTermsList() {
    const c = document.getElementById('termList');
    if (!c) return;
    c.innerHTML = SOLAR_TERMS.map(t => `
      <div class="term-cell ${S_COLOR[t.season]}" data-term="${t.name}">
        <div class="tm-top">
          <span class="tm-name">${t.name}</span>
          <span class="tm-emoji">${S_EMOJI[t.season]||'🌿'}</span>
        </div>
        <div class="tm-meta">📅 ${t.month}月${t.day}日 · ${t.climate}</div>
        <div class="tm-desc">${t.description}</div>
        <div class="tm-arrow">👉 查看详情</div>
      </div>`).join('');
  }

  function renderFestList() {
    const c = document.getElementById('festList');
    if (!c) return;
    c.innerHTML = FESTIVALS.map(f => `
      <div class="fest-cell" data-festival="${f.name}">
        <span class="fs-emoji">${F_EMOJI[f.name]||'🏮'}</span>
        <div class="fs-name">${f.name} <span style="font-size:12px;color:#999;font-weight:400;">${f.english}</span></div>
        <div class="fs-lunar">📅 农历：${f.lunarDate}</div>
        <div class="fs-solar">📆 ${year}年：${f.solarDates[year]||f.solarDates[2026]||''}</div>
        <div class="fs-desc">${f.description}</div>
        <div class="fs-arrow">👉 查看详情</div>
      </div>`).join('');
  }

  function renderShichenList() {
    const c = document.getElementById('scList');
    if (!c) return;
    const cur = getCurrentShiChen();
    c.innerHTML = SHI_CHEN.map(sc => {
      const isCur = cur.name === sc.name;
      return `
      <div class="sc-cell ${isCur?'current':''}" data-shichen="${sc.name}">
        <span class="sc-icon">${DZ_EMOJI[sc.diZhi]||'🕐'}</span>
        <div>
          <div class="sc-title">${sc.name} ${isCur?' 👈 当前':''}</div>
          <div class="sc-sub">${sc.range} · 属${sc.diZhi} · ${sc.animal}</div>
        </div>
      </div>`;
    }).join('');
  }

  function renderSeasonList() {
    const c = document.getElementById('seasonList');
    if (!c) return;
    c.innerHTML = SI_JI.map(s => `
      <div class="season-card" data-season="${s.name}" style="border-left:4px solid ${s.color};">
        <span class="sn-icon">${s.icon}</span>
        <div class="sn-title">${s.name}季</div>
        <div class="sn-desc">${s.description}</div>
        <div class="sn-health">🌿 养生：${s.healthTips}</div>
        <div class="sn-terms">
          ${s.solarTerms.map(st => `<span class="sn-term-tag" data-term="${st}">${st}</span>`).join('')}
        </div>
      </div>`).join('');
  }

  // ========== 弹窗系统 ==========
  function openDateDetail(dateStr) {
    const [y,m,d] = dateStr.split('-').map(Number);
    const st = getSolarTermForDate(m, d, y);
    const fests = getFestivalsForDate(y, m, d);
    const season = getSeasonForMonth(m);
    const yi = getYearTianGanDiZhi(y);
    const mi = getMonthTianGanDiZhi(y, m);

    let h = '<div class="modal-handle"></div><div class="modal-body">'+
      '<div class="modal-h1">📅 '+y+'年'+m+'月'+d+'日</div>'+
      '<div class="modal-h2">'+yi.fullName+' · '+mi.fullName+' · '+(S_EMOJI[season.name]||'')+' '+season.name+'季</div>';

    if (!st && fests.length === 0) {
      h += '<div class="modal-block" style="text-align:center;">'+
        '<p style="text-indent:0;">'+season.description+'</p></div>';
    }

    if (st) h += buildTermHTML(st);
    fests.forEach(f => { h += buildFestHTML(f, y); });
    h += '</div>';
    showModal(h);
  }

  function openTermDetail(name) {
    const st = getSolarTermByName(name);
    if (!st) return;
    showModal('<div class="modal-handle"></div><div class="modal-body">'+
      '<div class="modal-h1">'+(S_EMOJI[st.season]||'🌿')+' '+st.name+'</div>'+
      '<div class="modal-h2">'+st.pinyin+' · '+st.english+'</div>'+
      buildTermHTML(st)+'</div>');
  }

  function buildTermHTML(st) {
    return '<div class="modal-block"><div class="modal-block-title">📖 节气解说</div><p>'+st.description+'</p></div>'+
      '<div class="modal-block"><div class="modal-block-title">📜 历史背景</div><p>'+st.background+'</p></div>'+
      '<div class="modal-block"><div class="modal-block-title">🌤 气候特点</div><p style="text-indent:0;text-align:center;font-size:15px;font-weight:700;">'+st.climate+'</p></div>'+
      '<div class="modal-block"><div class="modal-block-title">🎋 传统习俗</div><p style="text-indent:0;text-align:center;">'+st.custom+'</p></div>'+
      '<div class="modal-block"><div class="modal-block-title">🍽️ 饮食推荐</div><div class="food-grid">'+
        st.dietary.map(d => '<div class="food-item"><div class="fi-name">🥢 '+d.food+'</div><div class="fi-reason">'+d.reason+'</div></div>').join('')+
      '</div></div>'+
      '<div class="modal-block"><div class="poem-box">📝 '+st.poems+'</div></div>';
  }

  function openFestDetail(name) {
    const f = FESTIVALS.find(x => x.name === name);
    if (!f) return;
    showModal('<div class="modal-handle"></div><div class="modal-body">'+
      '<div class="modal-h1">'+(F_EMOJI[f.name]||'🏮')+' '+f.name+'</div>'+
      '<div class="modal-h2">'+f.english+' · 农历'+f.lunarDate+'</div>'+
      buildFestHTML(f, year)+'</div>');
  }

  function buildFestHTML(f, y) {
    const sd = f.solarDates[y] || f.solarDates[2026] || '';
    return '<div class="modal-block"><div class="modal-block-title">📖 节日介绍</div><p>'+f.description+'</p></div>'+
      '<div class="modal-block"><div class="modal-block-title">📜 历史背景</div><p>'+f.background+'</p></div>'+
      (sd ? '<div class="modal-block"><div class="modal-block-title">📅 公历日期</div><p style="text-indent:0;text-align:center;font-size:18px;font-weight:900;color:#8B0000;">'+y+'年 '+sd+'</p></div>' : '')+
      '<div class="modal-block"><div class="modal-block-title">🎊 传统习俗</div><div class="chip-row">'+f.customs.map(c => '<span class="chip red">'+c+'</span>').join('')+'</div></div>'+
      '<div class="modal-block"><div class="modal-block-title">🍜 节日美食</div><div class="chip-row">'+f.foods.map(fd => '<span class="chip">'+fd+'</span>').join('')+'</div></div>'+
      (f.poems ? '<div class="modal-block"><div class="poem-box">📝 '+f.poems+'</div></div>' : '')+
      '<div class="modal-block" style="text-align:center;"><p style="text-indent:0;font-size:11px;color:#888;">📅 近年日期：'+Object.entries(f.solarDates).map(e => e[0]+'年'+e[1]).join(' · ')+'</p></div>';
  }

  function openYearDetail() {
    const info = getYearTianGanDiZhi(year);
    showModal('<div class="modal-handle"></div><div class="modal-body">'+
      '<div class="modal-h1">'+(Z_EMOJI[info.shengXiao]||'')+' '+info.fullName+'</div>'+
      '<div class="modal-h2">'+info.yinYang+' · 五行属'+info.wuXing+' · 生肖'+info.shengXiao+'</div>'+
      '<div class="modal-block"><div class="modal-block-title">📖 十天干</div><p style="text-indent:0;">'+
        TIAN_GAN.map((t,i)=>t+'('+TIAN_GAN_WUXING[i]+'·'+TIAN_GAN_YINYANG[i]+')').join(' · ')+
        '</p><p>天干是古代表示次序的符号，最早用于纪日，后扩展为纪年、纪月、纪时，与五行阴阳紧密相连。</p></div>'+
      '<div class="modal-block"><div class="modal-block-title">📖 十二地支</div><p style="text-indent:0;">'+
        DI_ZHI.map((d,i)=>d+'('+DI_ZHI_WUXING[i]+'·'+DI_ZHI_SHENGXIAO[i]+')').join(' · ')+
        '</p><p>地支源于木星公转周期（约12年），古人将天穹分为十二次，后与生肖结合形成独特文化。</p></div>'+
      '<div class="modal-block"><div class="modal-block-title">🔢 年干支计算</div>'+
        '<p style="text-indent:0;font-size:13px;text-align:center;">'+
        '天干：(年份-4)%10 = ('+year+'-4)%10 = '+(year-4)%10+' → <b>'+info.tianGan+'</b><br>'+
        '地支：(年份-4)%12 = ('+year+'-4)%12 = '+(year-4)%12+' → <b>'+info.diZhi+'</b></p></div>'+
      '</div>');
  }

  function openShichenModal() {
    const cur = getCurrentShiChen();
    let h = '<div class="modal-handle"></div><div class="modal-body">'+
      '<div class="modal-h1">🕐 十二时辰</div>'+
      '<div class="modal-h2">西周起源 · 三千年传承</div>'+
      '<div class="modal-block"><p style="text-indent:0;">十二时辰将一日分为十二等份，从子时(23:00起)依次排列。每个时辰对应一个地支、一种动物，反映古人对自然和生命的深刻观察。</p></div>';

    SHI_CHEN.forEach(sc => {
      const isCur = cur.name === sc.name;
      h += '<div class="time-card'+(isCur?' current':'')+'">'+
        '<span class="tc-emoji">'+(DZ_EMOJI[sc.diZhi]||'🕐')+'</span>'+
        '<div><div class="tc-name">'+sc.name+(isCur?' 👈 当前':'')+'</div>'+
        '<div class="tc-sub">'+sc.range+' · 地支：'+sc.diZhi+' · '+sc.animal+'</div>'+
        '<div class="tc-desc">'+sc.description+'</div></div></div>';
    });

    h += '</div>';
    showModal(h);
  }

  function openSeasonModal(name) {
    const s = getSeasonByName(name);
    if (!s) return;
    showModal('<div class="modal-handle"></div><div class="modal-body">'+
      '<div class="modal-h1">'+s.icon+' '+s.name+'季</div>'+
      '<div class="modal-h2">'+s.solarTerms.join(' · ')+'</div>'+
      '<div class="modal-block"><p>'+s.description+'</p></div>'+
      '<div class="modal-block"><div class="modal-block-title">🌿 养生指南</div><p style="text-indent:0;">'+s.healthTips+'</p></div>'+
      '<div class="modal-block"><div class="modal-block-title">📅 所属节气</div><div class="chip-row">'+
        s.solarTerms.map(st => '<span class="chip red" data-term="'+st+'">'+st+'</span>').join('')+
      '</div></div></div>');
  }

  function showModal(html) {
    closeModal();
    const ov = document.createElement('div');
    ov.className = 'modal-overlay';
    ov.innerHTML = '<div class="modal-sheet">'+html+'</div>';
    ov.addEventListener('click', function(e) {
      if (e.target === ov) closeModal();
    });

    const sheet = ov.querySelector('.modal-sheet');
    let startY = 0;
    sheet.addEventListener('touchstart', e => { startY = e.touches[0].clientY; }, { passive: true });
    sheet.addEventListener('touchmove', e => {
      if (sheet.scrollTop <= 0 && e.touches[0].clientY - startY > 80) closeModal();
    }, { passive: true });

    document.body.appendChild(ov);
    document.body.style.overflow = 'hidden';

    ov.querySelectorAll('.chip[data-term]').forEach(c => {
      c.addEventListener('click', () => openTermDetail(c.dataset.term));
    });
    ov.querySelectorAll('.sn-term-tag[data-term]').forEach(c => {
      c.addEventListener('click', e => { e.stopPropagation(); openTermDetail(c.dataset.term); });
    });
  }

  function closeModal() {
    document.querySelectorAll('.modal-overlay').forEach(m => m.remove());
    document.body.style.overflow = '';
  }

  // ========== 游戏系统 ==========
  const GAME = {
    score: 0,
    bestScore: 0,
    streak: 0,
    currentQuiz: null,
    currentQuizIndex: 0,
    quizQuestions: [],
    _quizCorrect: 0,
    currentWheelIndex: 0,
    selectedMatchTerm: null,
    matchPairs: {},
    sortOrder: [],
    sortPool: [],

    init() {
      const saved = localStorage.getItem('gameCalendarScore');
      if (saved) {
        try { const d = JSON.parse(saved); this.score = d.score || 0; this.bestScore = d.bestScore || 0; } catch(e) {}
      }
    },

    saveScore() {
      if (this.score > this.bestScore) this.bestScore = this.score;
      localStorage.setItem('gameCalendarScore', JSON.stringify({ score: this.score, bestScore: this.bestScore }));
    },

    addScore(pts) { this.score += pts; this.saveScore(); },

    renderHub() {
      const tg = document.getElementById('tabGame');
      if (!tg) return;
      tg.innerHTML = `<div class="page-head"><span class="ph-emoji">🎮</span><div class="ph-title">互动游戏</div><div class="ph-sub">学传统文化 · 赢趣味积分</div></div>
        <div class="score-panel"><div class="sp-left"><span class="sp-star">⭐</span><span class="sp-val" id="totalScore">${this.score}</span><span class="sp-label">总积分</span></div><div class="sp-right"><div class="sp-stat">🏆 最高: <span>${this.bestScore}</span></div></div></div>
        <div class="game-cards">
          <div class="game-card" id="gameGanzhiWheel"><div class="gc-emoji">☯️</div><div class="gc-name">天干地支轮盘</div><div class="gc-desc">探索六十甲子 · 触摸转动轮盘</div><div class="gc-tags"><span class="gc-tag">60甲子</span><span class="gc-tag">五行</span><span class="gc-tag">生肖</span></div></div>
          <div class="game-card" id="gameQuiz"><div class="gc-emoji">🧠</div><div class="gc-name">知识挑战</div><div class="gc-desc">节气 · 时辰 · 天干地支问答</div><div class="gc-tags"><span class="gc-tag">节气</span><span class="gc-tag">时辰</span><span class="gc-tag">干支</span></div></div>
          <div class="game-card" id="gameMatchTerm"><div class="gc-emoji">🔗</div><div class="gc-name">节气配对</div><div class="gc-desc">将节气与所属季节进行配对</div><div class="gc-tags"><span class="gc-tag">季节</span><span class="gc-tag">配对</span></div></div>
          <div class="game-card" id="gameSort"><div class="gc-emoji">📊</div><div class="gc-name">时间排序</div><div class="gc-desc">按正确顺序排列时间与时辰</div><div class="gc-tags"><span class="gc-tag">排序</span><span class="gc-tag">时辰</span></div></div>
        </div>`;
      var gw = document.getElementById('gameGanzhiWheel');
      var gq = document.getElementById('gameQuiz');
      var gm = document.getElementById('gameMatchTerm');
      var gs = document.getElementById('gameSort');
      if (gw) gw.onclick = function() { GAME.renderWheel(); };
      if (gq) gq.onclick = function() { GAME.renderQuizPicker(); };
      if (gm) gm.onclick = function() { GAME.renderMatch(); };
      if (gs) gs.onclick = function() { GAME.renderSort(); };
      window.scrollTo({ top: 0, behavior: 'smooth' });
    },

    backBtnHtml: '<div style="text-align:center;margin:12px 0 20px;"><button class="qr-back" onclick="window._gameBack()">↩ 返回游戏列表</button></div>',

    refreshScoreUI() {
      var el = document.getElementById('totalScore');
      if (el) el.textContent = this.score;
    },

    // ===== 天干地支轮盘 =====
    renderWheel() {
      this.currentWheelIndex = 0;
      var gz = GAN_ZHI_60[0];
      var ZE = { '鼠':'🐭','牛':'🐮','虎':'🐯','兔':'🐰','龙':'🐲','蛇':'🐍','马':'🐴','羊':'🐑','猴':'🐵','鸡':'🐔','狗':'🐶','猪':'🐷' };
      document.getElementById('tabGame').innerHTML = `<div class="page-head"><span class="ph-emoji">☯️</span><div class="ph-title">六十甲子轮盘</div><div class="ph-sub">天干地支循环纪年 · 滑动探索</div></div>
        <div class="wheel-container"><div class="wheel-scroll"><div class="wheel-items" id="wheelItems"></div></div></div>
        <div class="wheel-info" id="wheelInfo"><div class="wi-num">第${gz.index}位 / 共60</div><div class="wi-name">${gz.name}</div>
        <div class="wi-details"><span class="wi-tag">天干：${gz.tianGan}</span><span class="wi-tag">地支：${gz.diZhi}</span><span class="wi-tag">五行：${gz.wuXing}</span><span class="wi-tag">阴阳：${gz.yinYang}</span><span class="wi-tag">生肖：${ZE[gz.shengXiao]||''}${gz.shengXiao}</span></div>
        <div class="wi-btns"><button class="wi-btn prev" id="wheelPrev">◀ 上一个</button><button class="wi-btn next" id="wheelNext">下一个 ▶</button></div></div>${this.backBtnHtml}`;
      window._gameBack = function() { GAME.renderHub(); };
      this.initWheel();
      window.scrollTo({ top: 0, behavior: 'smooth' });
    },

    initWheel() {
      var container = document.getElementById('wheelItems');
      if (!container) return;
      container.innerHTML = GAN_ZHI_60.map(function(gz, i) { return '<div class="wheel-item" data-index="'+i+'"><span class="wi-num">'+gz.index+'</span><span class="wi-tg">'+gz.tianGan+'</span><span class="wi-dz">'+gz.diZhi+'</span></div>'; }).join('');
      var self = this;
      container.querySelectorAll('.wheel-item').forEach(function(item) {
        item.addEventListener('click', function() { self.currentWheelIndex = parseInt(this.dataset.index); self.updateWheelInfo(); self.scrollToWheelItem(this); });
      });
      var prev = document.getElementById('wheelPrev');
      var next = document.getElementById('wheelNext');
      if (prev) prev.onclick = function() { self.navigateWheel(-1); };
      if (next) next.onclick = function() { self.navigateWheel(1); };
      this.updateWheelInfo();
    },

    navigateWheel(delta) {
      this.currentWheelIndex = ((this.currentWheelIndex + delta) % 60 + 60) % 60;
      this.updateWheelInfo();
      var item = document.querySelector('.wheel-item[data-index="'+this.currentWheelIndex+'"]');
      if (item) this.scrollToWheelItem(item);
    },

    scrollToWheelItem(item) {
      item.scrollIntoView({ behavior: 'smooth', block: 'nearest', inline: 'center' });
      document.querySelectorAll('.wheel-item').forEach(function(el) { el.classList.remove('active'); });
      item.classList.add('active');
    },

    updateWheelInfo() {
      var gz = GAN_ZHI_60[this.currentWheelIndex];
      if (!gz) return;
      var ZE = { '鼠':'🐭','牛':'🐮','虎':'🐯','兔':'🐰','龙':'🐲','蛇':'🐍','马':'🐴','羊':'🐑','猴':'🐵','鸡':'🐔','狗':'🐶','猪':'🐷' };
      var wi = document.getElementById('wheelInfo');
      if (!wi) return;
      wi.querySelector('.wi-num').textContent = '第'+gz.index+'位 / 共60';
      wi.querySelector('.wi-name').textContent = gz.name;
      wi.querySelector('.wi-details').innerHTML = ['天干：'+gz.tianGan,'地支：'+gz.diZhi,'五行：'+gz.wuXing,'阴阳：'+gz.yinYang,'生肖：'+(ZE[gz.shengXiao]||'')+gz.shengXiao].map(function(s){return '<span class="wi-tag">'+s+'</span>';}).join('');
      document.querySelectorAll('.wheel-item').forEach(function(el){el.classList.remove('active');});
      var active = document.querySelector('.wheel-item[data-index="'+this.currentWheelIndex+'"]');
      if (active) { active.classList.add('active'); this.scrollToWheelItem(active); }
    },

    // ===== 知识挑战 =====
    renderQuizPicker() {
      document.getElementById('tabGame').innerHTML = `<div class="page-head"><span class="ph-emoji">🧠</span><div class="ph-title">知识挑战</div><div class="ph-sub">选择挑战类型开始答题</div></div>
        <div class="quiz-picker"><div class="qp-title">选择挑战类型：</div>
        <button class="qp-btn" data-quiz="termToSeason">🌿 节气→季节</button>
        <button class="qp-btn" data-quiz="seasonToTerm">🌸 季节→节气</button>
        <button class="qp-btn" data-quiz="termMonth">📅 节气→月份</button>
        <button class="qp-btn" data-quiz="termDesc">📖 描述→节气</button>
        <button class="qp-btn" data-quiz="shichenTime">🕐 时间→时辰</button>
        <button class="qp-btn" data-quiz="shichenDesc">⏰ 时辰→时间</button>
        <button class="qp-btn" data-quiz="ganzhiMatch">☯️ 干支→生肖</button>
        <button class="qp-btn" data-quiz="ganzhiWuxing">🔮 干支→五行</button>
        </div>${this.backBtnHtml}`;
      window._gameBack = function() { GAME.renderHub(); };
      var self = this;
      document.querySelectorAll('.qp-btn').forEach(function(btn) {
        btn.onclick = function() { self.startQuiz(this.dataset.quiz); };
      });
      window.scrollTo({ top: 0, behavior: 'smooth' });
    },

    startQuiz(type) {
      var questions = generateQuiz(type, 10);
      if (!questions.length) return;
      this.quizQuestions = questions;
      this.currentQuizIndex = 0;
      this.currentQuiz = type;
      this.streak = 0;
      this._quizCorrect = 0;
      var typeNames = { termToSeason:'节气→季节', seasonToTerm:'季节→节气', termMonth:'节气→月份', termDesc:'描述→节气', shichenTime:'时间→时辰', shichenDesc:'时辰→时间', ganzhiMatch:'干支→生肖', ganzhiWuxing:'干支→五行' };
      document.getElementById('navTitle').textContent = '🎮 '+(typeNames[type]||'知识挑战');
      this.renderQuizQuestion();
    },

    renderQuizQuestion() {
      if (this.currentQuizIndex >= this.quizQuestions.length) { this.renderQuizResult(); return; }
      var q = this.quizQuestions[this.currentQuizIndex];
      var total = this.quizQuestions.length;
      var idx = this.currentQuizIndex + 1;
      var pct = (idx/total)*100;
      document.getElementById('tabGame').innerHTML = `<div class="page-head"><span class="ph-emoji">🧠</span><div class="ph-title">知识挑战</div><div class="ph-sub">第${idx}/${total}题</div></div>
        <div class="quiz-progress"><div class="qp-bar-bg"><div class="qp-bar-fill" style="width:${pct}%;"></div></div><div class="qp-text">${idx}/${total}</div></div>
        <div class="quiz-question">${q.question}</div><div class="quiz-hint">💡 ${q.hint}</div>
        <div class="quiz-options" id="quizOptions"></div><div class="quiz-feedback" id="quizFeedback" style="display:none;"></div>${this.backBtnHtml}`;
      window._gameBack = function() { GAME.renderQuizPicker(); document.getElementById('navTitle').textContent = '🎮 互动游戏'; };
      var self = this;
      var oc = document.getElementById('quizOptions');
      q.options.forEach(function(opt) {
        var btn = document.createElement('div');
        btn.className = 'quiz-option';
        btn.textContent = opt;
        btn.addEventListener('click', function() { self.answerQuestion(opt, q); });
        oc.appendChild(btn);
      });
      window.scrollTo({ top: 0, behavior: 'smooth' });
    },

    answerQuestion(chosen, q) {
      var opts = document.querySelectorAll('.quiz-option');
      opts.forEach(function(b){ b.style.pointerEvents = 'none'; });
      var correct = chosen === q.answer;
      opts.forEach(function(b) {
        if (b.textContent === q.answer) b.classList.add('correct');
        else if (b.textContent === chosen && !correct) b.classList.add('wrong');
      });
      var fb = document.getElementById('quizFeedback');
      fb.style.display = 'block';
      if (correct) {
        fb.className = 'quiz-feedback correct';
        fb.textContent = '✅ 正确！'+(q.explain ? ' '+q.explain : '');
        this.streak++; this._quizCorrect++;
        this.addScore(10 + Math.min(this.streak-1, 5)*2);
      } else {
        fb.className = 'quiz-feedback wrong';
        fb.textContent = '❌ 答案：'+q.answer+(q.explain ? ' '+q.explain : '');
        this.streak = 0;
      }
      var self = this;
      setTimeout(function() { self.currentQuizIndex++; self.renderQuizQuestion(); }, 1800);
    },

    renderQuizResult() {
      var correct = this._quizCorrect||0;
      var total = this.quizQuestions.length;
      var score = Math.round((correct/total)*100);
      var msg = '再接再厉！';
      if (score===100) msg = '满分！你太厉害了！👑';
      else if (score>=80) msg = '非常棒！继续加油！⭐';
      else if (score>=60) msg = '还不错，再来一次！💪';
      var typeNames = { termToSeason:'节气→季节', seasonToTerm:'季节→节气', termMonth:'节气→月份', termDesc:'描述→节气', shichenTime:'时间→时辰', shichenDesc:'时辰→时间', ganzhiMatch:'干支→生肖', ganzhiWuxing:'干支→五行' };
      document.getElementById('tabGame').innerHTML = `<div class="page-head"><span class="ph-emoji">🧠</span><div class="ph-title">挑战完成！</div><div class="ph-sub">${typeNames[this.currentQuiz]||''} · 答对${correct}/${total}题</div></div>
        <div class="quiz-result" style="display:block;"><div class="qr-score">${score}分</div><div class="qr-msg">${msg}</div>
        <button class="qr-retry" id="qrRetry">🔄 再来一次</button><button class="qr-back" id="qrBackToPick">↩ 换类型</button></div>${this.backBtnHtml}`;
      window._gameBack = function() { GAME.renderHub(); document.getElementById('navTitle').textContent = '🎮 互动游戏'; };
      document.getElementById('qrRetry').onclick = function() { GAME.startQuiz(GAME.currentQuiz); };
      document.getElementById('qrBackToPick').onclick = function() { GAME.renderQuizPicker(); };
      document.getElementById('navTitle').textContent = '🎮 知识挑战';
      window.scrollTo({ top: 0, behavior: 'smooth' });
    },

    // ===== 节气配对 =====
    renderMatch() {
      this.matchPairs = {}; this.selectedMatchTerm = null;
      var allTerms = shuffleArray(SOLAR_TERMS.slice());
      document.getElementById('tabGame').innerHTML = `<div class="page-head"><span class="ph-emoji">🔗</span><div class="ph-title">节气配对</div><div class="ph-sub">点击节气 → 再点击季节</div></div>
        <div class="match-area"><div class="match-seasons" id="matchSeasons"></div><div class="match-terms" id="matchTerms"></div>
        <div class="match-score" id="matchScore">已配对: 0/24</div><div class="match-feedback" id="matchFeedback"></div>
        <button class="match-reset" id="matchReset">🔄 重新开始</button>
        <div class="match-done" id="matchDone" style="display:none;"><div class="md-emoji">🎉</div><div class="md-text">全部配对正确！</div><div class="md-score">+100 积分</div></div></div>${this.backBtnHtml}`;
      window._gameBack = function() { GAME.renderHub(); };
      this.initMatch(allTerms);
      window.scrollTo({ top: 0, behavior: 'smooth' });
    },

    initMatch(allTerms) {
      document.getElementById('matchSeasons').innerHTML = SI_JI.map(function(s){ return '<div class="match-season" data-season="'+s.name+'" style="background:'+s.color+';"><div class="ms-name">'+s.icon+' '+s.name+'季</div><div class="ms-count" id="msCount'+s.name+'">0/'+s.solarTerms.length+'</div><div class="ms-terms" id="msTerms'+s.name+'"></div></div>'; }).join('');
      document.getElementById('matchTerms').innerHTML = allTerms.map(function(t){ return '<div class="match-term-tag" data-term="'+t.name+'" data-season="'+t.season+'">'+t.name+'</div>'; }).join('');
      var self = this;
      document.querySelectorAll('.match-term-tag').forEach(function(tag){ tag.addEventListener('click', function(){ self.selectMatchTerm(this); }); });
      document.querySelectorAll('.match-season').forEach(function(s){ s.addEventListener('click', function(){ self.assignToSeason(this.dataset.season); }); });
      document.getElementById('matchReset').onclick = function() { GAME.renderMatch(); };
    },

    selectMatchTerm(tag) {
      document.querySelectorAll('.match-term-tag').forEach(function(t){ t.classList.remove('selected'); });
      tag.classList.add('selected');
      this.selectedMatchTerm = tag;
    },

    assignToSeason(seasonName) {
      var fb = document.getElementById('matchFeedback');
      if (!this.selectedMatchTerm) { fb.textContent = '请先点击上方选择一个节气'; fb.style.color = '#888'; return; }
      var termName = this.selectedMatchTerm.dataset.term;
      var correctSeason = this.selectedMatchTerm.dataset.season;
      if (correctSeason === seasonName) {
        this.matchPairs[termName] = seasonName;
        this.selectedMatchTerm.remove(); this.selectedMatchTerm = null;
        fb.textContent = '✅ 配对正确！'+termName+' → '+seasonName+'季'; fb.style.color = '#2E7D32';
        this.addScore(5);
        var count = Object.values(this.matchPairs).filter(function(v){ return v===seasonName; }).length;
        var s = SI_JI.find(function(s){ return s.name===seasonName; });
        document.getElementById('msCount'+seasonName).textContent = count+'/'+s.solarTerms.length;
        var el = document.getElementById('msTerms'+seasonName);
        var sp = document.createElement('span'); sp.className = 'ms-term-tag'; sp.textContent = termName;
        el.appendChild(sp);
        var total = Object.keys(this.matchPairs).length;
        document.getElementById('matchScore').textContent = '已配对: '+total+'/24';
        if (total >= 24) {
          fb.textContent = '';
          document.getElementById('matchDone').style.display = 'block';
          document.getElementById('matchTerms').style.display = 'none';
          this.addScore(100);
        }
      } else {
        fb.textContent = '❌ 不对！'+termName+'属于'+correctSeason+'季';
        fb.style.color = '#C62828';
        document.querySelectorAll('.match-term-tag').forEach(function(t){ t.classList.remove('selected'); });
        this.selectedMatchTerm = null;
      }
    },

    // ===== 时间排序 =====
    renderSort() {
      this.sortOrder = []; this.sortPool = shuffleArray(SHI_CHEN.slice());
      document.getElementById('tabGame').innerHTML = `<div class="page-head"><span class="ph-emoji">📊</span><div class="ph-title">时间排序</div><div class="ph-sub">按子时→亥时顺序排列时辰</div></div>
        <div class="sort-area"><div class="sort-desc">将下方时辰卡片按正确顺序排列：</div>
        <div class="sort-slots" id="sortSlots"></div><div class="sort-pool" id="sortPool"></div>
        <div class="sort-feedback" id="sortFeedback"></div><div class="sort-btns">
        <button class="sort-btn reset" id="sortReset">🔄 重新排列</button><button class="sort-btn check" id="sortCheck">✅ 提交检查</button></div>
        <div class="sort-done" id="sortDone" style="display:none;"><div class="md-emoji">🎉</div><div class="md-text">排列完全正确！</div><div class="md-score">+80 积分</div></div></div>${this.backBtnHtml}`;
      window._gameBack = function() { GAME.renderHub(); };
      this.initSort();
      window.scrollTo({ top: 0, behavior: 'smooth' });
    },

    initSort() {
      this.renderSortUI();
      document.getElementById('sortReset').onclick = function() { GAME.renderSort(); };
      document.getElementById('sortCheck').onclick = function() { GAME.checkSortOrder(); };
    },

    renderSortUI() {
      var slotsEl = document.getElementById('sortSlots');
      slotsEl.innerHTML = '';
      var self = this;
      this.sortOrder.forEach(function(sc, idx) {
        var chip = document.createElement('div'); chip.className = 'sort-chip';
        chip.textContent = sc.name+' '+sc.range;
        chip.addEventListener('click', function() { self.sortPool.push(self.sortOrder.splice(idx,1)[0]); self.renderSortUI(); });
        slotsEl.appendChild(chip);
      });
      var poolEl = document.getElementById('sortPool');
      poolEl.innerHTML = '';
      this.sortPool.forEach(function(sc) {
        var chip = document.createElement('div'); chip.className = 'sort-chip';
        chip.textContent = sc.name+' '+sc.range;
        chip.addEventListener('click', function() {
          var i = self.sortPool.indexOf(sc);
          if (i>=0) { self.sortPool.splice(i,1); self.sortOrder.push(sc); self.renderSortUI(); }
        });
        poolEl.appendChild(chip);
      });
    },

    checkSortOrder() {
      var fb = document.getElementById('sortFeedback');
      if (this.sortOrder.length !== SHI_CHEN.length) { fb.textContent = '⚠️ 请将所有时辰移到上方排序区'; fb.style.color = '#E65100'; return; }
      var correct = true;
      for (var i=0; i<SHI_CHEN.length; i++) { if (this.sortOrder[i].name !== SHI_CHEN[i].name) { correct=false; break; } }
      if (correct) { fb.textContent='✅ 完全正确！'; fb.style.color='#2E7D32'; document.getElementById('sortDone').style.display='block'; this.addScore(80); }
      else { fb.textContent='❌ 顺序不对，正确顺序：'+SHI_CHEN.map(function(s){return s.name;}).join(' → '); fb.style.color='#C62828'; }
    }
  };

  function bind() {
    document.getElementById('btnPrev').addEventListener('click', () => changeMonth(-1));
    document.getElementById('btnNext').addEventListener('click', () => changeMonth(1));
    document.getElementById('btnToday').addEventListener('click', goToday);
    document.getElementById('yearCard').addEventListener('click', openYearDetail);
    document.getElementById('shichenRow').addEventListener('click', openShichenModal);

    document.getElementById('dateGrid').addEventListener('click', function(e) {
      const cell = e.target.closest('.date-cell');
      if (cell && !cell.classList.contains('empty') && cell.dataset.date) {
        openDateDetail(cell.dataset.date);
      }
    });

    let tx = 0, ty = 0;
    document.getElementById('dateGrid').addEventListener('touchstart', e => {
      tx = e.touches[0].clientX; ty = e.touches[0].clientY;
    }, { passive: true });
    document.getElementById('dateGrid').addEventListener('touchend', e => {
      const dx = e.changedTouches[0].clientX - tx;
      const dy = e.changedTouches[0].clientY - ty;
      if (Math.abs(dx) > Math.abs(dy) && Math.abs(dx) > 50) {
        changeMonth(dx > 0 ? 1 : -1);
      }
    });

    document.querySelectorAll('.season-tab').forEach(t => {
      t.addEventListener('click', () => openSeasonModal(t.dataset.season));
    });

    document.querySelectorAll('.tab-item').forEach(t => {
      t.addEventListener('click', () => switchTab(t.dataset.tab));
    });

    document.getElementById('tabTerms').addEventListener('click', function(e) {
      const card = e.target.closest('.term-cell');
      if (card) openTermDetail(card.dataset.term);
    });

    document.getElementById('tabFestivals').addEventListener('click', function(e) {
      const card = e.target.closest('.fest-cell');
      if (card) openFestDetail(card.dataset.festival);
    });

    document.getElementById('tabShichen').addEventListener('click', function(e) {
      if (e.target.closest('.sc-cell')) openShichenModal();
    });

    document.getElementById('tabSeasons').addEventListener('click', function(e) {
      const tag = e.target.closest('.sn-term-tag');
      if (tag) { openTermDetail(tag.dataset.term); return; }
      const card = e.target.closest('.season-card');
      if (card) openSeasonModal(card.dataset.season);
    });

    document.addEventListener('keydown', function(e) {
      if (e.key === 'Escape') { closeModal(); return; }
      if (currentTab === 'tabCalendar' && e.key === 'ArrowLeft') changeMonth(-1);
      if (currentTab === 'tabCalendar' && e.key === 'ArrowRight') changeMonth(1);
    });
  }

  init();
})();
