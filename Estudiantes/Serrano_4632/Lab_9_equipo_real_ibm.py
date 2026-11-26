import json
import sys
from datetime import datetime, timezone

import numpy as np
from qiskit import QuantumCircuit, qasm2
from qiskit.transpiler import generate_preset_pass_manager
from qiskit_aer import AerSimulator
from qiskit_ibm_runtime import QiskitRuntimeService, SamplerV2

DISPAROS = 1000
ANGULOS_CHSH = {
    "a,b": (0.0, np.pi/4),
    "a,b'": (0.0, -np.pi/4),
    "a',b": (np.pi/2, np.pi/4),
    "a',b'": (np.pi/2, -np.pi/4),
}


def par_de_bell():
    qc = QuantumCircuit(2, 2)
    qc.h(0)
    qc.cx(0, 1)
    return qc


def circuitos():
    bell = par_de_bell()
    bell.barrier()
    bell.measure([0, 1], [0, 1])

    ghz = QuantumCircuit(3, 3)
    ghz.h(0)
    ghz.cx(0, 1)
    ghz.cx(1, 2)
    ghz.barrier()
    ghz.measure([0, 1, 2], [0, 1, 2])

    ghz_x = QuantumCircuit(3, 3)
    ghz_x.h(0)
    ghz_x.cx(0, 1)
    ghz_x.cx(1, 2)
    ghz_x.x(1)
    ghz_x.barrier()
    ghz_x.measure([0, 1, 2], [0, 1, 2])

    lista = {"Bell": bell, "GHZ": ghz, "GHZ + X(1)": ghz_x}
    for par, (theta_a, theta_b) in ANGULOS_CHSH.items():
        qc = par_de_bell()
        qc.ry(-theta_a, 0)
        qc.ry(-theta_b, 1)
        qc.barrier()
        qc.measure([0, 1], [0, 1])
        lista["CHSH " + par] = qc
    return lista


def ejecutar(equipo, ruta_salida, plan):
    lista = circuitos()
    nombres = list(lista)
    compilador = generate_preset_pass_manager(backend=equipo, optimization_level=3, seed_transpiler=2026)
    par = compilador.run(lista["Bell"]).layout.final_index_layout()
    trio = compilador.run(lista["GHZ"]).layout.final_index_layout()
    compilador_par = generate_preset_pass_manager(backend=equipo, optimization_level=3, seed_transpiler=2026,
                                                  initial_layout=par)
    compilador_trio = generate_preset_pass_manager(backend=equipo, optimization_level=3, seed_transpiler=2026,
                                                   initial_layout=trio)
    compilados = []
    for nombre in nombres:
        if lista[nombre].num_qubits == 2:
            compilados.append(compilador_par.run(lista[nombre]))
        else:
            compilados.append(compilador_trio.run(lista[nombre]))

    trabajo = SamplerV2(mode=equipo).run(compilados, shots=DISPAROS)
    resultado = trabajo.result()
    conteos_equipo = {}
    for i, nombre in enumerate(nombres):
        conteos_equipo[nombre] = resultado[i].data.c.get_counts()

    simulador_ruido = AerSimulator.from_backend(equipo)
    conteos_ruido = {}
    for i, nombre in enumerate(nombres):
        corrida = simulador_ruido.run(compilados[i], shots=DISPAROS, seed_simulator=2026 + i)
        conteos_ruido[nombre] = corrida.result().get_counts()

    qubits_fisicos = {}
    circuitos_equipo = {}
    for i, nombre in enumerate(nombres):
        qubits_fisicos[nombre] = [int(q) for q in compilados[i].layout.final_index_layout()]
        circuitos_equipo[nombre] = qasm2.dumps(compilados[i])

    uso = {}
    if hasattr(trabajo, "metrics"):
        metricas = trabajo.metrics()
        marcas = metricas["timestamps"]
        creado = datetime.fromisoformat(marcas["created"].replace("Z", "+00:00"))
        inicio = datetime.fromisoformat(marcas["running"].replace("Z", "+00:00"))
        fin = datetime.fromisoformat(marcas["finished"].replace("Z", "+00:00"))
        uso = {
            "creado_utc": marcas["created"],
            "inicio_utc": marcas["running"],
            "fin_utc": marcas["finished"],
            "cola_s": round((inicio - creado).total_seconds(), 1),
            "en_ejecucion_s": round((fin - inicio).total_seconds(), 1),
            "equipo_s": metricas["usage"]["qpu_charge_time_seconds"],
            "ru": metricas["usage"]["value"],
            "limite_s": 600,
            "limite_ru": 10,
        }

    registro = {
        "equipo": equipo.name,
        "qubits_equipo": equipo.num_qubits,
        "plan": plan,
        "trabajo": trabajo.job_id(),
        "fecha_utc": datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M"),
        "disparos": DISPAROS,
        "qubits_fisicos": qubits_fisicos,
        "circuitos_equipo": circuitos_equipo,
        "conteos_equipo": conteos_equipo,
        "conteos_ruido": conteos_ruido,
        "uso": uso,
    }
    with open(ruta_salida, "w", encoding="utf-8") as archivo:
        json.dump(registro, archivo, ensure_ascii=False, indent=1)
    print(f"{equipo.name}: trabajo {registro['trabajo']} -> {ruta_salida}")


if __name__ == "__main__":
    servicio = QiskitRuntimeService()
    abiertas = [instancia for instancia in servicio.instances() if "open" in str(instancia.get("plan", "")).lower()]
    if not abiertas:
        sys.exit("La cuenta no tiene una instancia del plan Open; no se envia nada.")
    servicio = QiskitRuntimeService(instance=abiertas[0]["crn"])
    print(servicio.usage())
    equipo = servicio.least_busy(operational=True, simulator=False, min_num_qubits=3)
    ejecutar(equipo, sys.argv[1] if len(sys.argv) > 1 else "Lab_9_equipo_real_ibm.json", abiertas[0]["plan"])
