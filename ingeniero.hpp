#ifndef COMPORTAMIENTOINGENIERO_H
#define COMPORTAMIENTOINGENIERO_H

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <time.h>

#include "comportamientos/comportamiento.hpp"

class ComportamientoIngeniero : public Comportamiento
{
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  enum EstadoN5
  {
    N5_INIT,
    N5_PLANIFICAR,
    N5_MOVERSE,
    N5_TRABAJAR,
    N5_ESPERAR,
    N5_INSTALL
  };
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoIngeniero(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado
    last_action = IDLE;
    tiene_zapatillas = false;
    giros_pendientes = 0;
    tuberiaPlanificada = false;
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoIngeniero(std::vector<std::vector<unsigned char>> mapaR,
                          std::vector<std::vector<unsigned char>> mapaC) : Comportamiento(mapaR, mapaC)
  {
    last_action = IDLE;
    tiene_zapatillas = false;
    giros_pendientes = 0;
    fase5 = 0;
    pipe_idx = 1;
    hayPlanN5 = false;
    anteriorIngF = -1;
    anteriorIngC = -1;
    esperaContador5 = 0;
    tuberiaPlanificada = false;
  }

  ComportamientoIngeniero(const ComportamientoIngeniero &comport)
      : Comportamiento(comport) {}
  ~ComportamientoIngeniero() {}

  /**
   * @brief Bucle principal de decisión del agente.
   * Estudia los sensores y decide la siguiente acción.
   *
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoIngeniero *clone()
  {
    return new ComportamientoIngeniero(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  // Funciones específicas para cada nivel (para ser implementadas por el alumno)

  /**
   * @brief Implementación del Nivel 0.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_0(Sensores sensores);

  /**
   * @brief Implementación del Nivel 1.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_1(Sensores sensores);

  /**
   * @brief Implementación del Nivel 2.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_2(Sensores sensores);

  /**
   * @brief Implementación del Nivel 3.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_3(Sensores sensores);

  /**
   * @brief Implementación del Nivel 4.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_4(Sensores sensores);

  /**
   * @brief Implementación del Nivel 5.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_5(Sensores sensores);
  std::list<Paso> BuscarCaminoINivel4(int belF, int belC, const std::vector<std::vector<unsigned char>> &mapaR, const std::vector<std::vector<unsigned char>> &mapaC, int maxEco);
  std::list<Action> BuscarCaminoI(const EstadoN5 &inicio, const EstadoN5 &fin, const std::vector<std::vector<unsigned char>> &terreno, const std::vector<std::vector<unsigned char>> &altura);
  std::list<Paso> BuscarPlanTuberias(Sensores sensores);

  // Función BFS para moverse (Declaración)
  std::list<Action> BFS_Mover_Universal(
      int fOrig, int cOrig, int rumboOrig, bool zap,
      int fDest, int cDest, const std::vector<std::vector<unsigned char>> &mapa);

  /**
   * @brief Implementación del Nivel 6.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza la información del mapa interno basándose en los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores (casilla actual + 15 casillas alrededor).
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Comprueba si una casilla es transitable.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee zapatillas.
   * @return true si la casilla es transitable (no es muro ni precipicio).
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLAS: Desnivel máximo 1 sin zapatillas, 2 con zapatillas.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual, bool zap);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

  bool es_camino(unsigned char c) const;

  /**
   * @brief Imprime por consola la secuencia de acciones de un plan para un agente.
   * @param plan  Lista de acciones del plan.
   */
  void PintaPlan(const list<Action> &plan);

  /**
   * @brief Imprime las coordenadas y operaciones de un plan de tubería.
   * @param plan  Lista de pasos (fila, columna, operación).
   */
  void PintaPlan(const list<Paso> &plan);

  /**
   * @brief Convierte un plan de acciones en una lista de casillas para
   *        su visualización en el mapa gráfico.
   * @param st    Estado de partida.
   * @param plan  Lista de acciones del plan.
   */
  void VisualizaPlan(const ubicacion &st, const list<Action> &plan);

  /**
   * @brief Convierte un plan de tubería en la lista de casillas usada
   *        por el sistema de visualización.
   * @param st    Estado de partida (no utilizado directamente).
   * @param plan  Lista de pasos del plan de tubería.
   */
  void VisualizaRedTuberias(const list<Paso> &plan);

private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================

  Action last_action;
  bool tiene_zapatillas;
  int giros_pendientes;
  bool hayPlanN2;
  list<Action> planN2;

  // Estado Nivel 5
  EstadoN5 eN5;
  bool hayPlanN5;
  int anteriorIngF, anteriorIngC;
  std::list<Paso> planTuberiasN5;
  std::list<Action> planMovN5;
  std::list<Paso> BuscarTuberias(int f, int c, int maxEco);
  std::list<Action> BFS_Simple(int fO, int cO, int rO, int fD, int cD);
  int pipe_idx;
  int fase5;
  int esperaContador5;
  bool tuberiaPlanificada;
};

#endif
