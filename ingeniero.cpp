#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>
#include <climits>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoIngenieroNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoIngenieroNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoIngenieroNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoIngenieroNivel_3(sensores);
    break;
  case 4:
    accion = ComportamientoIngenieroNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoIngenieroNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoIngenieroNivel_6(sensores);
    break;
  }

  return accion;
}
char ViablePorAlturaI(char casilla, int dif, bool zap)
{
  if (abs(dif) <= 1 or (zap and abs(dif) <= 2))
    return casilla;
  else
    return 'P';
}

int VisitasI_Nivel0(int f, int c, int brujula, const vector<vector<int>> &mapa)
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

int VeoCasillaInteresanteI_Nivel0(char i, char c, char d, bool zap, int v_i, int v_c, int v_d)
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
int PrioridadN0_I(char casilla)
{
  if (casilla == 'U')
    return 3;
  if (casilla == 'D')
    return 2;
  if (casilla == 'C' || casilla == 'S')
    return 1;
  return 0;
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
  if (sensores.superficie[0] == 'U')
    return IDLE;

  static int reversaI = 0;
  if (sensores.agentes[2] == 't')
    reversaI = 4;
  if (reversaI > 0)
  {
    reversaI--;
    return TURN_SR;
  }

  static vector<vector<int>> mapaVisitas;
  if (mapaVisitas.empty() || sensores.reset)
  {
    mapaVisitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
    reversaI = 0;
  }
  mapaVisitas[sensores.posF][sensores.posC]++;
  ActualizarMapa(sensores);
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  auto EvaluarI = [&](int idx)
  {
    char tipo = sensores.superficie[idx];
    int diff = abs(sensores.cota[idx] - sensores.cota[0]);
    bool zap = tiene_zapatillas;

    if (!((!zap && diff < 2) || (zap && diff < 3)))
      return 0;

    if (tipo == 'U')
    {
      if (sensores.agentes[idx] == 't')
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
    p[i] = EvaluarI(i);
    v[i] = VisitasI_Nivel0(sensores.posF, sensores.posC, (sensores.rumbo + (i == 1 ? 7 : (i == 2 ? 0 : 1))) % 8, mapaVisitas);
  }

  int mejor = 2;
  if (p[1] > p[mejor] || (p[1] == p[mejor] && v[1] < v[mejor]))
    mejor = 1;
  if (p[3] > p[mejor] || (p[3] == p[mejor] && v[3] < v[mejor]))
    mejor = 3;

  if (p[mejor] == 0)
    return TURN_SL;
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

bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

int VisitasI(int f, int c, int brujula, const vector<vector<int>> &mapa)
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

int VeoCasillaInteresanteI_Nivel1(char i, char c, char d, bool zap, int v_i, int v_c, int v_d)
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

/**
 * @brief Comportamiento reactivo del ingeniero para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
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

  char i = ViablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = ViablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = ViablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  int v_i = VisitasI(sensores.posF, sensores.posC, (sensores.rumbo + 7) % 8, mapaVisitados);
  int v_c = VisitasI(sensores.posF, sensores.posC, sensores.rumbo, mapaVisitados);
  int v_d = VisitasI(sensores.posF, sensores.posC, (sensores.rumbo + 1) % 8, mapaVisitados);

  int pos = VeoCasillaInteresanteI_Nivel1(i, c, d, tiene_zapatillas, v_i, v_c, v_d);

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

// Niveles avanzados (Uso de búsqueda)

/**
 * @brief Comportamiento del ingeniero para el Nivel 2 (búsqueda).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
  static list<Action> plan;
  static bool hay_plan = false;

  if (sensores.reset)
  {
    hay_plan = false;
    plan.clear();
  }

  if (!hay_plan)
  {
    struct Estado
    {
      int f, c, brujula;
      bool zap;
      bool operator<(const Estado &o) const
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

    struct Nodo
    {
      Estado st;
      list<Action> secuencia;
    };

    queue<Nodo> abiertos;
    set<Estado> cerrados;

    Estado inicial;
    inicial.f = sensores.posF;
    inicial.c = sensores.posC;
    inicial.brujula = sensores.rumbo;
    inicial.zap = (mapaResultado[sensores.posF][sensores.posC] == 'D' || tiene_zapatillas);

    abiertos.push({inicial, list<Action>()});
    cerrados.insert(inicial);

    while (!abiertos.empty())
    {
      Nodo actual = abiertos.front();
      abiertos.pop();

      if (actual.st.f == sensores.BelPosF && actual.st.c == sensores.BelPosC)
      {
        plan = actual.secuencia;
        hay_plan = true;
        break;
      }

      Estado h_SL = actual.st;
      h_SL.brujula = (actual.st.brujula + 7) % 8;
      if (cerrados.find(h_SL) == cerrados.end())
      {
        cerrados.insert(h_SL);
        Nodo n = actual;
        n.st = h_SL;
        n.secuencia.push_back(TURN_SL);
        abiertos.push(n);
      }

      Estado h_SR = actual.st;
      h_SR.brujula = (actual.st.brujula + 1) % 8;
      if (cerrados.find(h_SR) == cerrados.end())
      {
        cerrados.insert(h_SR);
        Nodo n = actual;
        n.st = h_SR;
        n.secuencia.push_back(TURN_SR);
        abiertos.push(n);
      }

      pair<int, int> p_w;
      p_w.first = actual.st.f;
      p_w.second = actual.st.c;

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

      bool transitable_w = (p_w.first >= 0 && p_w.first < mapaResultado.size() && p_w.second >= 0 && p_w.second < mapaResultado[0].size() && mapaResultado[p_w.first][p_w.second] != 'P' && mapaResultado[p_w.first][p_w.second] != 'M' && mapaResultado[p_w.first][p_w.second] != 'B');

      if (transitable_w)
      {
        int dif = abs(mapaCotas[p_w.first][p_w.second] - mapaCotas[actual.st.f][actual.st.c]);
        if ((!actual.st.zap && dif <= 1) || (actual.st.zap && dif <= 2))
        {
          Estado h_W = actual.st;
          h_W.f = p_w.first;
          h_W.c = p_w.second;
          if (mapaResultado[h_W.f][h_W.c] == 'D')
            h_W.zap = true;

          if (cerrados.find(h_W) == cerrados.end())
          {
            cerrados.insert(h_W);
            Nodo n = actual;
            n.st = h_W;
            n.secuencia.push_back(WALK);
            abiertos.push(n);
          }
        }

        pair<int, int> p_j;
        p_j.first = p_w.first;
        p_j.second = p_w.second;

        switch (actual.st.brujula)
        {
        case 0:
          p_j.first--;
          break;
        case 1:
          p_j.first--;
          p_j.second++;
          break;
        case 2:
          p_j.second++;
          break;
        case 3:
          p_j.first++;
          p_j.second++;
          break;
        case 4:
          p_j.first++;
          break;
        case 5:
          p_j.first++;
          p_j.second--;
          break;
        case 6:
          p_j.second--;
          break;
        case 7:
          p_j.first--;
          p_j.second--;
          break;
        }

        bool transitable_j = (p_j.first >= 0 && p_j.first < mapaResultado.size() && p_j.second >= 0 && p_j.second < mapaResultado[0].size() && mapaResultado[p_j.first][p_j.second] != 'P' && mapaResultado[p_j.first][p_j.second] != 'M' && mapaResultado[p_j.first][p_j.second] != 'B');

        if (transitable_j)
        {
          int dif_j = abs(mapaCotas[p_j.first][p_j.second] - mapaCotas[actual.st.f][actual.st.c]);
          if ((!actual.st.zap && dif_j <= 1) || (actual.st.zap && dif_j <= 2))
          {
            Estado h_J = actual.st;
            h_J.f = p_j.first;
            h_J.c = p_j.second;
            if (mapaResultado[h_J.f][h_J.c] == 'D')
              h_J.zap = true;

            if (cerrados.find(h_J) == cerrados.end())
            {
              cerrados.insert(h_J);
              Nodo n = actual;
              n.st = h_J;
              n.secuencia.push_back(JUMP);
              abiertos.push(n);
            }
          }
        }
      }
    }
  }

  Action accion_a_realizar = IDLE;
  if (hay_plan && !plan.empty())
  {
    accion_a_realizar = plan.front();

    if (accion_a_realizar == WALK && sensores.agentes[2] == 't')
      return IDLE;
    if (accion_a_realizar == JUMP && sensores.agentes[6] == 't')
      return IDLE;

    plan.pop_front();
  }

  return accion_a_realizar;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores)
{
  bool veo_tecnico = false;

  for (int i = 1; i < sensores.agentes.size(); i++)
  {
    if (sensores.agentes[i] == 't')
    {
      veo_tecnico = true;
      break;
    }
  }

  if (veo_tecnico)
  {
    char sup = sensores.superficie[2];
    char ag = sensores.agentes[2];
    int dif = sensores.cota[2] - sensores.cota[0];

    if (ag == '_' && sup != 'M' && sup != 'P' && sup != 'B' && abs(dif) <= 1)
    {
      return WALK;
    }
    else
    {
      return TURN_SR;
    }
  }

  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

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

int CosteInstall(char terreno)
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

int CosteDigRaise(Action accion, char terreno)
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

int ImpactoDigRaise(Action accion, char terreno)
{
  return CosteDigRaise(accion, terreno);
}

int HeuristicaN4(int f, int c, const vector<pair<int, int>> &plantas_U)
{
  int min_h = 1000000;
  for (auto &u : plantas_U)
    min_h = min(min_h, abs(f - u.first) + abs(c - u.second));
  return min_h;
}

bool GravedadValida(int h_actual, int h_siguiente)
{
  return (h_actual >= h_siguiente) && (h_actual - h_siguiente) <= 1;
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
  priority_queue<NodoN4, vector<NodoN4>, greater<NodoN4>> abiertos;
  map<EstadoN4, int> cerrados;

  int max_ecologico = sensores.max_ecologico;
  int max_energia = sensores.energia;

  vector<pair<int, int>> plantas_U;
  for (int i = 0; i < (int)mapaResultado.size(); i++)
    for (int j = 0; j < (int)mapaResultado[0].size(); j++)
      if (mapaResultado[i][j] == 'U')
        plantas_U.push_back({i, j});

  char bTerr = mapaResultado[sensores.BelPosF][sensores.BelPosC];

  for (int op = -1; op <= 1; op++)
  {
    if (bTerr == 'A' && op != 0)
      continue;

    int h_ini = (int)mapaCotas[sensores.BelPosF][sensores.BelPosC] + op;
    if (h_ini < 0 || h_ini > 9)
      continue;
    if (abs(op) > 1)
      continue;

    int eco_ini = 0;
    int ene_ini = 0;

    if (op == -1)
    {
      eco_ini += ImpactoDigRaise(DIG, bTerr);
      ene_ini += CosteDigRaise(DIG, bTerr);
    }
    else if (op == 1)
    {
      eco_ini += ImpactoDigRaise(RAISE, bTerr);
      ene_ini += CosteDigRaise(RAISE, bTerr);
    }
    ene_ini += CosteInstall(bTerr);

    if (eco_ini > max_ecologico || ene_ini > max_energia)
      continue;

    EstadoN4 ini = {sensores.BelPosF, sensores.BelPosC, h_ini};
    NodoN4 nodo_ini;
    nodo_ini.st = ini;
    nodo_ini.plan = {{sensores.BelPosF, sensores.BelPosC, op}};
    nodo_ini.impacto_eco = eco_ini;
    nodo_ini.energia = ene_ini;
    nodo_ini.g = 1;
    nodo_ini.heuristica = HeuristicaN4(sensores.BelPosF, sensores.BelPosC, plantas_U);
    abiertos.push(nodo_ini);
  }

  while (!abiertos.empty())
  {
    NodoN4 actual = abiertos.top();
    abiertos.pop();

    if (mapaResultado[actual.st.f][actual.st.c] == 'U')
    {
      VisualizaRedTuberias(actual.plan);
      return IDLE;
    }

    if (cerrados.count(actual.st) &&
        cerrados[actual.st] <= actual.impacto_eco)
      continue;
    cerrados[actual.st] = actual.impacto_eco;

    int df[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, 1, -1};

    for (int i = 0; i < 4; i++)
    {
      int nf = actual.st.f + df[i];
      int nc = actual.st.c + dc[i];

      if (nf < 0 || nf >= (int)mapaResultado.size() ||
          nc < 0 || nc >= (int)mapaResultado[0].size())
        continue;

      char terr = mapaResultado[nf][nc];
      if (terr == 'P' || terr == 'M' || terr == 'B')
        continue;

      int op_min = -1, op_max = 1;
      if (terr == 'A')
      {
        op_min = 0;
        op_max = 0;
      }

      char terr_actual = mapaResultado[actual.st.f][actual.st.c];

      for (int op = op_min; op <= op_max; op++)
      {
        int nh = (int)mapaCotas[nf][nc] + op;
        if (nh < 0 || nh > 9)
          continue;
        if (abs(nh - (int)mapaCotas[nf][nc]) > 1)
          continue;

        if (!GravedadValida(actual.st.h, nh))
          continue;

        int coste_seg = 0;
        int impacto_seg = 0;

        if (op == -1)
        {
          coste_seg += CosteDigRaise(DIG, terr);
          impacto_seg += ImpactoDigRaise(DIG, terr);
        }
        else if (op == 1)
        {
          coste_seg += CosteDigRaise(RAISE, terr);
          impacto_seg += ImpactoDigRaise(RAISE, terr);
        }

        coste_seg += CosteInstall(terr);
        impacto_seg += CosteInstall(terr_actual);
        impacto_seg += CosteInstall(terr);

        int n_eco = actual.impacto_eco + impacto_seg;
        int n_ene = actual.energia + coste_seg;

        if (n_eco > max_ecologico || n_ene > max_energia)
          continue;

        EstadoN4 hijo_st = {nf, nc, nh};

        if (cerrados.count(hijo_st) &&
            cerrados[hijo_st] <= n_eco)
          continue;

        NodoN4 hijo = actual;
        hijo.st = hijo_st;
        hijo.plan.push_back({nf, nc, op});
        hijo.impacto_eco = n_eco;
        hijo.energia = n_ene;
        hijo.g = actual.g + 1;
        hijo.heuristica = HeuristicaN4(nf, nc, plantas_U);
        abiertos.push(hijo);
      }
    }
  }

  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

static Orientacion Orientacion_Hacia(int f1, int c1, int f2, int c2)
{
  if (f2 < f1 && c2 == c1)
    return norte;
  if (f2 < f1 && c2 > c1)
    return noreste;
  if (f2 == f1 && c2 > c1)
    return este;
  if (f2 > f1 && c2 > c1)
    return sureste;
  if (f2 > f1 && c2 == c1)
    return sur;
  if (f2 > f1 && c2 < c1)
    return suroeste;
  if (f2 == f1 && c2 < c1)
    return oeste;
  if (f2 < f1 && c2 < c1)
    return noroeste;
  return norte;
}

static Action Girar_hacia(int actual, int deseada)
{
  int dist = (deseada - actual + 8) % 8;
  return (dist <= 4) ? TURN_SR : TURN_SL;
}

list<Action> ComportamientoIngeniero::BFS_Simple(int fO, int cO, int rO, int fD, int cD)
{
  struct State
  {
    int f, c, r;
    bool operator<(const State &o) const
    {
      if (f != o.f)
        return f < o.f;
      if (c != o.c)
        return c < o.c;
      return r < o.r;
    }
  };
  queue<pair<State, list<Action>>> q;
  q.push({{fO, cO, rO}, {}});
  set<State> vis;
  while (!q.empty())
  {
    auto curr = q.front();
    q.pop();
    if (curr.first.f == fD && curr.first.c == cD)
      return curr.second;
    if (vis.count(curr.first))
      continue;
    vis.insert(curr.first);
    auto sl = curr;
    sl.first.r = (sl.first.r + 7) % 8;
    sl.second.push_back(TURN_SL);
    q.push(sl);
    auto sr = curr;
    sr.first.r = (sr.first.r + 1) % 8;
    sr.second.push_back(TURN_SR);
    q.push(sr);
    int df[] = {-1, -1, 0, 1, 1, 1, 0, -1}, dc[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int nf = curr.first.f + df[curr.first.r], nc = curr.first.c + dc[curr.first.r];
    if (nf >= 0 && nf < (int)mapaResultado.size() && nc >= 0 && nc < (int)mapaResultado[0].size())
    {
      char t = mapaResultado[nf][nc];
      int dif = abs(mapaCotas[nf][nc] - mapaCotas[curr.first.f][curr.first.c]);
      bool h_ok = (!tiene_zapatillas && dif <= 1) || (tiene_zapatillas && dif <= 2);
      if (t != 'P' && t != 'M' && t != 'B' && t != '?' && h_ok)
      {
        auto w = curr;
        w.first.f = nf;
        w.first.c = nc;
        w.second.push_back(WALK);
        q.push(w);
      }
    }
  }
  return {};
}

list<Paso> ComportamientoIngeniero::BuscarTuberias(int belF, int belC, int maxEco)
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
    NodoN4 n;
    n.st = {belF, belC, h_ini};
    n.plan = {{belF, belC, op}};
    n.impacto_eco = (op == -1 ? CosteDigRaise(DIG, bTerr) : (op == 1 ? CosteDigRaise(RAISE, bTerr) : 0));
    n.g = 1;
    n.heuristica = HeuristicaN4(belF, belC, plantas_U);
    abiertos.push(n);
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
          NodoN4 h = actual;
          h.st = {nf, nc, nh};
          h.plan.push_back({nf, nc, op});
          h.impacto_eco += i_seg;
          h.g++;
          h.heuristica = HeuristicaN4(nf, nc, plantas_U);
          abiertos.push(h);
        }
      }
    }
  }
  return {};
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores)
{
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (fase5 == 0)
  {
    if (planTuberiasN5.empty())
    {
      planTuberiasN5 = BuscarTuberias(sensores.BelPosF, sensores.BelPosC, sensores.max_ecologico);
      if (planTuberiasN5.empty())
        return IDLE;
      VisualizaRedTuberias(planTuberiasN5);
      pipe_idx = 1;
      hayPlanN5 = false;
      planMovN5.clear();
    }

    Paso primera = planTuberiasN5.front();
    if (sensores.posF == primera.fil && sensores.posC == primera.col)
    {
      fase5 = 1;
    }
    else
    {
      if (!hayPlanN5)
      {
        planMovN5 = BFS_Simple(sensores.posF, sensores.posC, sensores.rumbo, primera.fil, primera.col);
        hayPlanN5 = !planMovN5.empty();
      }
      if (!planMovN5.empty())
      {
        Action a = planMovN5.front();
        planMovN5.pop_front();
        if (planMovN5.empty())
          hayPlanN5 = false;
        return a;
      }
      return IDLE;
    }
  }

  if (fase5 == 1)
  {
    hayPlanN5 = false;
    planMovN5.clear();
    fase5 = 2;
    return COME;
  }

  if (pipe_idx >= (int)planTuberiasN5.size())
    return IDLE;

  auto it = planTuberiasN5.begin();
  advance(it, pipe_idx);
  Paso casillaActual = *it;

  Paso casillaAnterior = *prev(it);

  if (fase5 == 2)
  {
    if (sensores.posF == casillaActual.fil && sensores.posC == casillaActual.col)
    {
      fase5 = 3;
    }
    else
    {
      if (!hayPlanN5)
      {
        planMovN5 = BFS_Simple(sensores.posF, sensores.posC, sensores.rumbo, casillaActual.fil, casillaActual.col);
        hayPlanN5 = !planMovN5.empty();
      }
      if (!planMovN5.empty())
      {
        Action a = planMovN5.front();
        planMovN5.pop_front();
        if (planMovN5.empty())
          hayPlanN5 = false;
        return a;
      }
    }
  }

  if (fase5 == 3)
  {
    fase5 = 4;
    if (casillaActual.op == 1)
      return RAISE;
    if (casillaActual.op == -1)
      return DIG;
  }

  if (fase5 == 4)
  {
    int df = casillaAnterior.fil - sensores.posF;
    int dc = casillaAnterior.col - sensores.posC;
    Orientacion target;
    if (df == -1 && dc == 0)
      target = norte;
    else if (df == -1 && dc == 1)
      target = noreste;
    else if (df == 0 && dc == 1)
      target = este;
    else if (df == 1 && dc == 1)
      target = sureste;
    else if (df == 1 && dc == 0)
      target = sur;
    else if (df == 1 && dc == -1)
      target = suroeste;
    else if (df == 0 && dc == -1)
      target = oeste;
    else if (df == -1 && dc == -1)
      target = noroeste;
    else
      target = norte;

    if (sensores.rumbo != target)
    {
      int diff = ((int)target - (int)sensores.rumbo + 8) % 8;
      return (diff <= 4) ? TURN_SR : TURN_SL;
    }
    fase5 = 5;
  }

  if (fase5 == 5)
  {
    if (sensores.enfrente)
    {
      pipe_idx++;
      fase5 = (pipe_idx >= (int)planTuberiasN5.size()) ? 6 : 1;
      hayPlanN5 = false;
      planMovN5.clear();
      return INSTALL;
    }
    return IDLE;
  }

  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  ActualizarMapa(sensores);
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (!tuberiaPlanificada)
  {
    if (sensores.BelPosF != -1 &&
        mapaResultado[sensores.BelPosF][sensores.BelPosC] != '?')
    {

      planTuberiasN5 = BuscarTuberias(
          sensores.BelPosF, sensores.BelPosC, sensores.max_ecologico);

      bool todas_conocidas = true;
      for (const Paso &p : planTuberiasN5)
      {
        if (mapaResultado[p.fil][p.col] == '?')
        {
          todas_conocidas = false;
          break;
        }
      }

      if (!planTuberiasN5.empty() && todas_conocidas)
      {
        tuberiaPlanificada = true;
        VisualizaRedTuberias(planTuberiasN5);

        fase5 = 0;
        pipe_idx = 1;
        planMovN5.clear();
        hayPlanN5 = false;
      }
      else
      {
        planTuberiasN5.clear();
      }
    }
  }

  if (tuberiaPlanificada)
  {
    return ComportamientoIngenieroNivel_5(sensores);
  }

  return ComportamientoIngenieroNivel_1(sensores);
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
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
 * @brief Determina si una casilla es transitable para el ingeniero.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable (no es muro ni precipicio).
 */
bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el ingeniero: desnivel máximo 1 sin zapatillas, 2 con zapatillas.
 * @param actual Estado actual del agente (fila, columna, orientacion, zap).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
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
void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
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
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 *
 * @param plan  Lista de pasos (fila, columna, operación),
 *              donde operacion = -1 (DIG), operación = 1 (RAISE).
 */
void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
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
void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st,
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

/**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 *
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
