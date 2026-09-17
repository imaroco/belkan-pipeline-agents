#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <map>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoTecnicoNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoTecnicoNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoTecnicoNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoTecnicoNivel_3(sensores);
    break;
  case 4:
    accion = ComportamientoTecnicoNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoTecnicoNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoTecnicoNivel_6(sensores);
    break;
  }

  return accion;
}
char ViablePorAlturaT(char casilla, int dif)
{
  if (abs(dif) <= 1)
    return casilla;
  else
    return 'P';
}

int VisitasT_Nivel0(int f, int c, int brujula, const vector<vector<int>> &mapa)
{
  switch (brujula)
  {
  case 0:
    f--;
    break;
  case 1:
    f--;
    c++;
    break;
  case 2:
    c++;
    break;
  case 3:
    f++;
    c++;
    break;
  case 4:
    f++;
    break;
  case 5:
    f++;
    c--;
    break;
  case 6:
    c--;
    break;
  case 7:
    f--;
    c--;
    break;
  }
  if (f >= 0 && f < (int)mapa.size() && c >= 0 && c < (int)mapa[0].size())
  {
    return mapa[f][c];
  }
  return 9999;
}

int VeoCasillaInteresanteT_Nivel0(char i, char c, char d, bool zap, int v_i, int v_c, int v_d)
{
  int min_v = 9999;
  int mejor = 0;
  bool ok_c = false;
  if ((!zap && c == 'D') || c == 'C' || c == 'S' || c == 'U')
    ok_c = true;
  bool ok_i = false;
  if ((!zap && i == 'D') || i == 'C' || i == 'S' || i == 'U')
    ok_i = true;
  bool ok_d = false;
  if ((!zap && d == 'D') || d == 'C' || d == 'S' || d == 'U')
    ok_d = true;

  if (ok_c && v_c < min_v)
  {
    min_v = v_c;
    mejor = 2;
  }
  if (ok_i && v_i < min_v)
  {
    min_v = v_i;
    mejor = 1;
  }
  if (ok_d && v_d < min_v)
  {
    min_v = v_d;
    mejor = 3;
  }
  return mejor;
}
int PrioridadN0_T(char casilla)
{
  if (casilla == 'U')
    return 3;
  if (casilla == 'D')
    return 2;
  if (casilla == 'C' || casilla == 'S')
    return 1;
  return 0;
}

Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores)
{
  if (sensores.superficie[0] == 'U')
    return IDLE;

  static int reversaT = 0;
  if (sensores.agentes[2] == 'i')
    reversaT = 4;
  if (reversaT > 0)
  {
    reversaT--;
    return TURN_SL;
  }

  static vector<vector<int>> mapaVisitasT;
  if (mapaVisitasT.empty() || sensores.reset)
  {
    mapaVisitasT.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
    reversaT = 0;
  }
  mapaVisitasT[sensores.posF][sensores.posC]++;
  ActualizarMapa(sensores);
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  auto EvaluarT = [&](int idx)
  {
    char tipo = sensores.superficie[idx];
    int diff = abs(sensores.cota[idx] - sensores.cota[0]);
    if (diff > 1)
      return 0; // Técnico limitado a 1[cite: 4, 5]

    if (tipo == 'U')
    {
      // CORRECCIÓN: Solo bloquea si el Ingeniero está físicamente ahí (idx)
      if (sensores.agentes[idx] == 'i')
        return 0;
      return 3;
    }
    if (tipo == 'D')
      return 2;
    if (tipo == 'C')
      return 1;
    return 0;
  };

  int p[4], v[4];
  for (int i : {1, 2, 3})
  {
    p[i] = EvaluarT(i);
    v[i] = VisitasT_Nivel0(sensores.posF, sensores.posC, (sensores.rumbo + (i == 1 ? 7 : (i == 2 ? 0 : 1))) % 8, mapaVisitasT);
  }

  int mejor = 2;
  if (p[1] > p[mejor] || (p[1] == p[mejor] && v[1] < v[mejor]))
    mejor = 1;
  if (p[3] > p[mejor] || (p[3] == p[mejor] && v[3] < v[mejor]))
    mejor = 3;

  if (p[mejor] == 0)
    return TURN_SR;
  switch (mejor)
  {
  case 2:
    return WALK;
  case 1:
    return TURN_SL;
  case 3:
    return TURN_SR;
  }
  return IDLE;
}

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoTecnico::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}
int VisitasT(int f, int c, int brujula, const vector<vector<int>> &mapa)
{
  switch (brujula)
  {
  case 0:
    f--;
    break;
  case 1:
    f--;
    c++;
    break;
  case 2:
    c++;
    break;
  case 3:
    f++;
    c++;
    break;
  case 4:
    f++;
    break;
  case 5:
    f++;
    c--;
    break;
  case 6:
    c--;
    break;
  case 7:
    f--;
    c--;
    break;
  }

  if (f >= 0 && f < mapa.size() && c >= 0 && c < mapa[0].size())
  {
    return mapa[f][c];
  }

  return 9999;
}

/**
 * @brief Comportamiento reactivo del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
int VeoCasillaInteresanteT_Nivel1(char i, char c, char d, bool zap, int v_i, int v_c, int v_d)
{
  int min_v = 9999;
  int mejor = 0;

  bool ok_c = false;
  if ((!zap && c == 'D') || c == 'C' || c == 'S' || c == 'U')
  {
    ok_c = true;
  }

  bool ok_i = false;
  if ((!zap && i == 'D') || i == 'C' || i == 'S' || i == 'U')
  {
    ok_i = true;
  }

  bool ok_d = false;
  if ((!zap && d == 'D') || d == 'C' || d == 'S' || d == 'U')
  {
    ok_d = true;
  }

  if (ok_c)
  {
    if (v_c < min_v)
    {
      min_v = v_c;
      mejor = 2;
    }
  }

  if (ok_i)
  {
    if (v_i < min_v)
    {
      min_v = v_i;
      mejor = 1;
    }
  }

  if (ok_d)
  {
    if (v_d < min_v)
    {
      min_v = v_d;
      mejor = 3;
    }
  }

  return mejor;
}
Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores)
{
  static vector<vector<int>> mapaVisitados;

  if (mapaVisitados.empty() || sensores.reset)
  {
    mapaVisitados.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }

  mapaVisitados[sensores.posF][sensores.posC]++;

  Action accion = IDLE;
  ActualizarMapa(sensores);

  if (sensores.superficie[0] == 'D')
  {
    tiene_zapatillas = true;
  }

  if (giros_pendientes > 0)
  {
    giros_pendientes--;
    accion = last_action;
    last_action = accion;
    return accion;
  }

  char i = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

  int v_i = VisitasT(sensores.posF, sensores.posC, (sensores.rumbo + 7) % 8, mapaVisitados);
  int v_c = VisitasT(sensores.posF, sensores.posC, sensores.rumbo, mapaVisitados);
  int v_d = VisitasT(sensores.posF, sensores.posC, (sensores.rumbo + 1) % 8, mapaVisitados);

  int pos = VeoCasillaInteresanteT_Nivel1(i, c, d, tiene_zapatillas, v_i, v_c, v_d);

  if (pos == 0)
  {
    accion = TURN_SL;
    giros_pendientes = 3;
  }
  else
  {
    switch (pos)
    {
    case 2:
      accion = WALK;
      break;
    case 1:
      accion = TURN_SL;
      break;
    case 3:
      accion = TURN_SR;
      break;
    }
  }

  if (accion == WALK)
  {
    if (sensores.agentes[1] != '_' || sensores.agentes[2] != '_' || sensores.agentes[3] != '_')
    {
      if (sensores.superficie[2] != 'U')
      {
        accion = TURN_SL;
        giros_pendientes = 3;
      }
    }
  }

  last_action = accion;
  return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores)
{
  bool veo_ingeniero = false;
  for (int i = 1; i < sensores.agentes.size(); i++)
  {
    if (sensores.agentes[i] == 'i')
    {
      veo_ingeniero = true;
      break;
    }
  }

  if (veo_ingeniero)
  {
    return ComportamientoTecnicoNivel_0(sensores);
  }

  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
int DistanciaDiagonal(int f1, int c1, int f2, int c2)
{
  return max(abs(f1 - f2), abs(c1 - c2));
}

int CosteGiroT(char terreno)
{
  if (terreno == 'A')
    return 5;
  if (terreno == 'H')
    return 2;
  return 1;
}

int CosteAvanceT(char terreno, int dif_altura)
{
  int coste = 0;

  if (terreno == 'A')
    coste = 60;
  else if (terreno == 'H')
    coste = 6;
  else if (terreno == 'S')
    coste = 3;
  else
    return 1;

  if (dif_altura > 0)
    coste += 5;
  else if (dif_altura < 0)
    coste -= 2;

  return coste;
}

struct EstadoN3T
{
  int f, c, brujula;
  bool zap;
  bool operator<(const EstadoN3T &o) const
  {
    if (f != o.f)
      return f < o.f;
    if (c != o.c)
      return c < o.c;
    if (brujula != o.brujula)
      return brujula < o.brujula;
    return zap < o.zap;
  }
};

struct NodoN3T
{
  EstadoN3T st;
  list<Action> secuencia;
  int coste_acumulado;
  int heuristica;

  bool operator>(const NodoN3T &o) const
  {
    int f_this = coste_acumulado + heuristica;
    int f_other = o.coste_acumulado + o.heuristica;
    return f_this > f_other;
  }
};

Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores)
{
  ActualizarMapa(sensores);

  if (sensores.reset)
  {
    hayPlan = false;
    plan.clear();
    tiene_zapatillas = false;
  }

  if (sensores.superficie[0] == 'D')
  {
    tiene_zapatillas = true;
  }

  if (!hayPlan)
  {
    priority_queue<NodoN3T, vector<NodoN3T>, greater<NodoN3T>> abiertos;
    set<EstadoN3T> cerrados;

    EstadoN3T inicial;
    inicial.f = sensores.posF;
    inicial.c = sensores.posC;
    inicial.brujula = sensores.rumbo;
    inicial.zap = tiene_zapatillas;

    int h_ini = DistanciaDiagonal(inicial.f, inicial.c,
                                  sensores.BelPosF, sensores.BelPosC);
    abiertos.push({inicial, list<Action>(), 0, h_ini});

    while (!abiertos.empty())
    {
      NodoN3T actual = abiertos.top();
      abiertos.pop();

      if (cerrados.find(actual.st) != cerrados.end())
        continue;
      cerrados.insert(actual.st);

      if (actual.st.f == sensores.BelPosF &&
          actual.st.c == sensores.BelPosC)
      {
        plan = actual.secuencia;
        hayPlan = true;
        break;
      }

      char terreno_actual = mapaResultado[actual.st.f][actual.st.c];

      EstadoN3T h_SL = actual.st;
      h_SL.brujula = (actual.st.brujula + 7) % 8;
      if (cerrados.find(h_SL) == cerrados.end())
      {
        NodoN3T n = actual;
        n.st = h_SL;
        n.secuencia.push_back(TURN_SL);
        n.coste_acumulado += CosteGiroT(terreno_actual);
        abiertos.push(n);
      }

      EstadoN3T h_SR = actual.st;
      h_SR.brujula = (actual.st.brujula + 1) % 8;
      if (cerrados.find(h_SR) == cerrados.end())
      {
        NodoN3T n = actual;
        n.st = h_SR;
        n.secuencia.push_back(TURN_SR);
        n.coste_acumulado += CosteGiroT(terreno_actual);
        abiertos.push(n);
      }

      pair<int, int> p_w = {actual.st.f, actual.st.c};
      switch (actual.st.brujula)
      {
      case 0:
        p_w.first--;
        break;
      case 1:
        p_w.first--;
        p_w.second++;
        break;
      case 2:
        p_w.second++;
        break;
      case 3:
        p_w.first++;
        p_w.second++;
        break;
      case 4:
        p_w.first++;
        break;
      case 5:
        p_w.first++;
        p_w.second--;
        break;
      case 6:
        p_w.second--;
        break;
      case 7:
        p_w.first--;
        p_w.second--;
        break;
      }

      bool dentro = (p_w.first >= 0 &&
                     p_w.first < (int)mapaResultado.size() &&
                     p_w.second >= 0 &&
                     p_w.second < (int)mapaResultado[0].size());

      if (dentro)
      {
        char ter_dest = mapaResultado[p_w.first][p_w.second];

        bool bloqueado = (ter_dest == 'P' || ter_dest == 'M');
        if (ter_dest == 'B' && !actual.st.zap)
          bloqueado = true;

        if (!bloqueado)
        {
          int dif = mapaCotas[p_w.first][p_w.second] - mapaCotas[actual.st.f][actual.st.c];

          if (abs(dif) <= 1)
          {
            EstadoN3T h_W = actual.st;
            h_W.f = p_w.first;
            h_W.c = p_w.second;
            if (ter_dest == 'D')
              h_W.zap = true;

            if (cerrados.find(h_W) == cerrados.end())
            {
              NodoN3T n = actual;
              n.st = h_W;
              n.secuencia.push_back(WALK);
              n.coste_acumulado += CosteAvanceT(terreno_actual, dif);
              n.heuristica = DistanciaDiagonal(
                  h_W.f, h_W.c,
                  sensores.BelPosF, sensores.BelPosC);
              abiertos.push(n);
            }
          }
        }
      }
    }
  }

  Action accion = IDLE;
  if (hayPlan && !plan.empty())
  {
    accion = plan.front();

    if (accion == WALK && sensores.agentes[2] != '_')
    {
      return IDLE;
    }

    plan.pop_front();
  }

  if (plan.empty())
  {
    hayPlan = false;
  }

  return accion;
}
struct EstadoN4
{
  int f, c, h;
  bool operator<(const EstadoN4 &o) const
  {
    if (f != o.f)
      return f < o.f;
    if (c != o.c)
      return c < o.c;
    return h < o.h;
  }
};

struct NodoN4
{
  EstadoN4 st;
  list<Paso> plan;
  int impacto_eco;
  int energia;
  int g;
  int heuristica;

  bool operator>(const NodoN4 &o) const
  {
    if ((g + heuristica) != (o.g + o.heuristica))
      return (g + heuristica) > (o.g + o.heuristica);
    return impacto_eco > o.impacto_eco;
  }
};

static int CosteInstall(char terreno)
{
  if (terreno == 'A')
    return 50;
  else if (terreno == 'H')
    return 45;
  else if (terreno == 'S')
    return 25;
  else if (terreno == 'C' || terreno == 'U')
    return 15;
  return 30;
}

static int CosteDigRaise(Action accion, char terreno)
{
  if (accion == DIG)
  {
    if (terreno == 'H')
      return 65;
    else if (terreno == 'S')
      return 40;
    else if (terreno == 'C' || terreno == 'U')
      return 25;
    return 50;
  }
  else
  {
    if (terreno == 'H')
      return 55;
    else if (terreno == 'S')
      return 30;
    else if (terreno == 'C' || terreno == 'U')
      return 10;
    return 40;
  }
}

static int ImpactoDigRaise(Action accion, char terreno)
{
  return CosteDigRaise(accion, terreno);
}

static int HeuristicaN4(int f, int c, const vector<pair<int, int>> &plantas_U)
{
  int min_h = 1000000;
  for (auto &u : plantas_U)
    min_h = min(min_h, abs(f - u.first) + abs(c - u.second));
  return min_h;
}

static bool GravedadValida(int h_actual, int h_siguiente)
{
  return (h_actual >= h_siguiente) && (h_actual - h_siguiente) <= 1;
}
/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

list<Paso> ComportamientoTecnico::BuscarTuberias(int belF, int belC, int maxEco)
{
  priority_queue<NodoN4, vector<NodoN4>, greater<NodoN4>> abiertos;
  map<EstadoN4, int> cerrados;
  vector<pair<int, int>> plantas_U;
  for (int i = 0; i < (int)mapaResultado.size(); i++)
    for (int j = 0; j < (int)mapaResultado[0].size(); j++)
      if (mapaResultado[i][j] == 'U')
        plantas_U.push_back({i, j});

  char bTerr = mapaResultado[belF][belC];
  for (int op = -1; op <= 1; op++)
  {
    int h_ini = (int)mapaCotas[belF][belC] + op;
    if (h_ini < 0 || h_ini > 9 || (bTerr == 'A' && op != 0))
      continue;
    NodoN4 nodo_ini;
    nodo_ini.st = {belF, belC, h_ini};
    nodo_ini.plan = {{belF, belC, op}};
    nodo_ini.impacto_eco = (op == -1 ? CosteDigRaise(DIG, bTerr) : (op == 1 ? CosteDigRaise(RAISE, bTerr) : 0));
    nodo_ini.g = 1;
    nodo_ini.heuristica = HeuristicaN4(belF, belC, plantas_U);
    abiertos.push(nodo_ini);
  }

  while (!abiertos.empty())
  {
    NodoN4 actual = abiertos.top();
    abiertos.pop();
    if (mapaResultado[actual.st.f][actual.st.c] == 'U')
      return actual.plan;
    if (cerrados.count(actual.st) && cerrados[actual.st] <= actual.impacto_eco)
      continue;
    cerrados[actual.st] = actual.impacto_eco;

    int df[] = {-1, 1, 0, 0}, dc[] = {0, 0, 1, -1};
    for (int i = 0; i < 4; i++)
    {
      int nf = actual.st.f + df[i], nc = actual.st.c + dc[i];
      if (nf < 0 || nf >= (int)mapaResultado.size() || nc < 0 || nc >= (int)mapaResultado[0].size())
        continue;
      char terr = mapaResultado[nf][nc];
      if (terr == 'P' || terr == 'M' || terr == 'B')
        continue;

      int dif_suelo = abs((int)mapaCotas[nf][nc] - (int)mapaCotas[actual.st.f][actual.st.c]);
      if (dif_suelo > 1)
        continue;

      for (int op = -1; op <= 1; op++)
      {
        int nh = (int)mapaCotas[nf][nc] + op;
        if (nh < 0 || nh > 9 || !GravedadValida(actual.st.h, nh))
          continue;
        int i_seg = (op == -1 ? CosteDigRaise(DIG, terr) : (op == 1 ? CosteDigRaise(RAISE, terr) : 0)) + CosteInstall(mapaResultado[actual.st.f][actual.st.c]) + CosteInstall(terr);
        if (actual.impacto_eco + i_seg <= maxEco)
        {
          NodoN4 hijo = actual;
          hijo.st = {nf, nc, nh};
          hijo.plan.push_back({nf, nc, op});
          hijo.impacto_eco += i_seg;
          hijo.g++;
          hijo.heuristica = HeuristicaN4(nf, nc, plantas_U);
          abiertos.push(hijo);
        }
      }
    }
  }
  return {};
}
struct StateN5
{
  int f, c, o;
  bool zap;
  bool operator<(const StateN5 &other) const
  {
    if (f != other.f)
      return f < other.f;
    if (c != other.c)
      return c < other.c;
    if (o != other.o)
      return o < other.o;
    return zap < other.zap;
  }
};

list<Action> ComportamientoTecnico::BFS_N5(
    int fO, int cO, int rO, bool zapO, int fD, int cD,
    int evitarF, int evitarC,
    const vector<vector<unsigned char>> &mapa,
    const vector<vector<unsigned char>> &alturas)
{
  queue<pair<StateN5, list<Action>>> q;
  set<StateN5> visited;
  StateN5 start = {fO, cO, rO, zapO};
  q.push({start, {}});
  visited.insert(start);

  while (!q.empty())
  {
    auto [cur, path] = q.front();
    q.pop();
    if (cur.f == fD && cur.c == cD)
      return path;

    StateN5 sl = {cur.f, cur.c, (cur.o + 7) % 8, cur.zap};
    if (visited.find(sl) == visited.end())
    {
      visited.insert(sl);
      auto p = path;
      p.push_back(TURN_SL);
      q.push({sl, p});
    }
    StateN5 sr = {cur.f, cur.c, (cur.o + 1) % 8, cur.zap};
    if (visited.find(sr) == visited.end())
    {
      visited.insert(sr);
      auto p = path;
      p.push_back(TURN_SR);
      q.push({sr, p});
    }

    int df[] = {-1, -1, 0, 1, 1, 1, 0, -1}, dc[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int nf = cur.f + df[cur.o], nc = cur.c + dc[cur.o];

    if (nf >= 0 && nf < (int)mapa.size() && nc >= 0 && nc < (int)mapa[0].size())
    {
      char celda = mapa[nf][nc];
      int diff = abs(alturas[nf][nc] - alturas[cur.f][cur.c]);

      bool esMuro = false;
      if (evitarF != -1)
      {
        esMuro = (nf == evitarF && nc == evitarC && !(nf == fD && nc == cD));
      }

      if (celda != 'P' && celda != 'M' && !esMuro && diff <= 1)
      {
        if (celda == 'B' && !cur.zap)
          continue;

        StateN5 nxt = {nf, nc, cur.o, cur.zap};
        if (celda == 'D')
          nxt.zap = true;
        if (visited.find(nxt) == visited.end())
        {
          visited.insert(nxt);
          auto p = path;
          p.push_back(WALK);
          q.push({nxt, p});
        }
      }
    }
  }
  return {};
}

Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores)
{
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (sensores.venpaca && fase_tec5 == 0)
  {
    if (!planCalculadoN5)
    {
      planTuberiasN5 = BuscarTuberias(sensores.BelPosF, sensores.BelPosC, sensores.max_ecologico);
      planCalculadoN5 = true;
      if (planTuberiasN5.size() >= 2)
      {
        auto it = planTuberiasN5.begin();
        ++it;
        evitarF = it->fil;
        evitarC = it->col;
      }
      else
      {
        evitarF = -1;
        evitarC = -1;
      }
    }
    goto5_f = sensores.GotoF;
    goto5_c = sensores.GotoC;
    planMovT.clear();
    hayPlan_tec5 = false;
    fase_tec5 = 1;
  }

  if (fase_tec5 == 0)
    return IDLE;

  if (fase_tec5 == 1)
  {
    if (sensores.posF == goto5_f && sensores.posC == goto5_c)
    {
      fase_tec5 = 2;
    }
    else
    {
      if (!hayPlan_tec5)
      {
        planMovT = BFS_N5(
            sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas,
            goto5_f, goto5_c,
            evitarF, evitarC,
            mapaResultado, mapaCotas);
        hayPlan_tec5 = !planMovT.empty();
      }
      if (!planMovT.empty())
      {
        Action a = planMovT.front();
        if (a == WALK)
        {
          int df[] = {-1, -1, 0, 1, 1, 1, 0, -1};
          int dc[] = {0, 1, 1, 1, 0, -1, -1, -1};
          int nf = sensores.posF + df[sensores.rumbo];
          int nc = sensores.posC + dc[sensores.rumbo];
          if (nf < 0 || nf >= (int)mapaResultado.size() || nc < 0 || nc >= (int)mapaResultado[0].size() ||
              mapaResultado[nf][nc] == 'P' || mapaResultado[nf][nc] == 'M' ||
              (mapaResultado[nf][nc] == 'B' && !tiene_zapatillas) ||
              mapaResultado[nf][nc] == '?')
          {
            planMovT.clear();
            hayPlan_tec5 = false;
            return IDLE;
          }
        }
        if (a == WALK && sensores.agentes[2] == 'i')
        {
          planMovT.clear();
          hayPlan_tec5 = false;
          return IDLE;
        }
        planMovT.pop_front();
        if (planMovT.empty())
          hayPlan_tec5 = false;
        return a;
      }
      return IDLE;
    }
  }

  if (fase_tec5 == 2)
  {
    if (sensores.enfrente)
    {
      fase_tec5 = 3;
    }
    else
    {
      return TURN_SR;
    }
  }

  if (fase_tec5 == 3)
  {
    fase_tec5 = 0;
    return INSTALL;
  }

  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores)
{
  ActualizarMapa(sensores);
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  static bool tuve_orden = false;
  if (sensores.venpaca)
    tuve_orden = true;

  if (tuve_orden || fase_tec5 > 0)
  {
    return ComportamientoTecnicoNivel_5(sensores);
  }
  return ComportamientoTecnicoNivel_1(sensores);
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoTecnico::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
        mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
      }
    break;
  case noreste:
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
    mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
    mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
    mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
    mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
    mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
    mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
    mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
    mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
    break;
  case este:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
        mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
      }
    break;
  case sureste:
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
    mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
    mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
    mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
    mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
    mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
    mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
    mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
    mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
    mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
    mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
    break;
  case sur:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
        mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
      }
    break;
  case suroeste:
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
    mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
    mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
    mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
    mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
    mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
    mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
    mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
    mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
    break;
  case oeste:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
        mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
      }
    break;
  case noroeste:
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
    mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
    mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
    mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
    mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
    mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
    mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
    mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
    mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
    mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
    mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
    break;
  }
}

/**
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break; // norte
  case 1:
    delante.f--;
    delante.c++;
    break; // noreste
  case 2:
    delante.c++;
    break; // este
  case 3:
    delante.f++;
    delante.c++;
    break; // sureste
  case 4:
    delante.f++;
    break; // sur
  case 5:
    delante.f++;
    delante.c--;
    break; // suroeste
  case 6:
    delante.c--;
    break; // oeste
  case 7:
    delante.f--;
    delante.c--;
    break; // noroeste
  }
  return delante;
}

/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
                                          const list<Action> &plan)
{
  listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion)(((int)cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion)(((int)cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}
