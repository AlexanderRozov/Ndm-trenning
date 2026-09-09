import os

PROJECT_NAME = "keenetic-firmware"

# Описание структуры проекта
STRUCTURE = {
    # Корневые файлы
    "README.md": f"# {PROJECT_NAME}\n\nРазработка кастомной прошивки.",
    "Makefile": "# Makefile для сборки прошивки\n.PHONY: all build clean\nall:\n\t@echo 'Запустите make build'\nbuild:\n\t@echo 'Сборка...'\nclean:\n\t@echo 'Очистка...'",
    "Dockerfile": "FROM ubuntu:22.04\nRUN apt-get update && apt-get install -y build-essential cmake\nCMD [\"/bin/bash\"]",
    "docker-compose.yml": "version: '3.8'\nservices:\n  builder:\n    build: .\n    volumes:\n      - .:/workspace\n    working_dir: /workspace",
    
    # Пустые папки для инфраструктуры сборки
    "build": {},
    "buildroot": {},
    "kernel/linux": {},
    "uboot": {},
    
    # Конфигурационные файлы
    "configs/device.conf": "# Конфигурация устройства\nboard=keenetic\n",
    "configs/network.conf": "# Сетевые настройки по умолчанию\nlan_ip=192.168.1.1\n",
    "configs/services.conf": "# Настройки системных служб\ndhcp_enabled=true\n",
    
    # Rootfs окружение
    "rootfs/etc/init.d": {},
    "rootfs/etc/network": {},
    "rootfs/etc/keenetic": {},
    "rootfs/bin": {},
    "rootfs/sbin": {},
    "rootfs/lib": {},
    "rootfs/usr": {},
    "rootfs/var": {},
    
    # Исходный код (C++)
    "src/common/logger/Logger.h": "", "src/common/logger/Logger.cpp": "",
    "src/common/config/Config.h": "", "src/common/config/Config.cpp": "",
    "src/common/events/Event.h": "", "src/common/events/EventQueue.h": "", "src/common/events/EventQueue.cpp": "",
    "src/common/process/Process.h": "", "src/common/process/Process.cpp": "",
    
    "src/keeneticd/main.cpp": '#include "KeeneticDaemon.h"\n\nint main() {\n    KeeneticDaemon daemon;\n    daemon.run();\n    return 0;\n}',
    "src/keeneticd/KeeneticDaemon.h": "", "src/keeneticd/KeeneticDaemon.cpp": "",
    
    "src/network/NetworkManager.h": "", "src/network/NetworkManager.cpp": "",
    "src/network/Interface.h": "", "src/network/Interface.cpp": "",
    "src/network/RouteManager.h": "", "src/network/RouteManager.cpp": "",
    "src/network/DhcpManager.h": "", "src/network/DhcpManager.cpp": "",
    
    "src/wifi/WifiManager.h": "", "src/wifi/WifiManager.cpp": "",
    "src/wifi/WifiInterface.h": "", "src/wifi/WifiInterface.cpp": "",
    
    "src/firewall/FirewallManager.h": "", "src/firewall/FirewallManager.cpp": "",
    
    "src/modem/ModemManager.h": "", "src/modem/ModemManager.cpp": "",
    "src/modem/Modem.h": "", "src/modem/Modem.cpp": "",
    "src/modem/AtChannel.h": "", "src/modem/AtChannel.cpp": "",
    "src/modem/QmiModem.h": "", "src/modem/QmiModem.cpp": "",
    "src/modem/MbimModem.h": "", "src/modem/MbimModem.cpp": "",
    
    "src/usb/UsbManager.h": "", "src/usb/UsbManager.cpp": "",
    
    "src/services/ServiceManager.h": "", "src/services/ServiceManager.cpp": "",
    "src/services/Watchdog.h": "", "src/services/Watchdog.cpp": "",
    
    "src/diagnostics/Diagnostics.h": "", "src/diagnostics/Diagnostics.cpp": "",
    "src/diagnostics/NetworkDiagnostics.h": "", "src/diagnostics/NetworkDiagnostics.cpp": "",
    
    "src/api/ApiServer.h": "", "src/api/ApiServer.cpp": "",
    "src/api/RouterApi.h": "", "src/api/RouterApi.cpp": "",
    
    # Тесты
    "tests/unit/network": {}, "tests/unit/modem": {}, "tests/unit/config": {}, "tests/unit/events": {},
    "tests/integration/network": {}, "tests/integration/modem": {}, "tests/integration/api": {},
    
    # Скрипты автоматизации
    "scripts/build.sh": "#!/bin/bash\necho 'Building...'",
    "scripts/run-qemu.sh": "#!/bin/bash\necho 'Starting QEMU...'",
    "scripts/stop-qemu.sh": "#!/bin/bash\necho 'Stopping QEMU...'",
    "scripts/create-image.sh": "#!/bin/bash\necho 'Creating image...'",
    "scripts/test-network.sh": "#!/bin/bash\necho 'Running network tests...'",
    
    # QEMU скрипты
    "qemu/run.sh": "#!/bin/bash\n",
    "qemu/network.sh": "#!/bin/bash\n",
    "qemu/qemu-ifup": "#!/bin/bash\n",
    
    # Документация
    "docs/architecture.md": "# Architecture",
    "docs/networking.md": "# Networking Stack",
    "docs/modem.md": "# Modem Subsystem (AT/QMI/MBIM)",
    "docs/boot.md": "# Bootloader & Kernel Boot Sequence",
    "docs/testing.md": "# Testing Guide"
}

def generate_cpp_content(file_path):
    """Генерирует дефолтное наполнение для C++ файлов"""
    filename = os.path.basename(file_path)
    name, ext = os.path.splitext(filename)
    
    if ext == ".h":
        macro = f"KEENETIC_{name.upper()}_H"
        return f"#ifndef {macro}\n#define {macro}\n\nclass {name} {{\npublic:\n    {name}() = default;\n    ~{name}() = default;\n}};\n\n#endif // {macro}\n"
    elif ext == ".cpp":
        return f'#include "{name}.h"\n'
    return ""

def main():
    root_path = os.path.abspath(PROJECT_NAME)
    print(f"--- Создание архитектуры прошивки в: {root_path} ---\n")
    
    for rel_path, default_content in STRUCTURE.items():
        full_path = os.path.join(root_path, rel_path)
        
        # Если это чистая папка (пустой dict)
        if isinstance(default_content, dict):
            os.makedirs(full_path, exist_ok=True)
            continue
            
        # Создаем родительские директории для файла
        os.makedirs(os.path.dirname(full_path), exist_ok=True)
        
        # Наполнение файла умными заглушками
        content = default_content
        if not content and (rel_path.endswith(".h") or rel_path.endswith(".cpp")):
            content = generate_cpp_content(rel_path)
            
        with open(full_path, "w", encoding="utf-8") as f:
            f.write(content)
            
        # Даем права на исполнение скриптам (.sh и qemu-ifup)
        if rel_path.endswith(".sh") or "qemu-ifup" in rel_path:
            try:
                os.chmod(full_path, 0o755)
            except AttributeError:
                pass # Пропускаем на Windows, если chmod не поддерживается в таком виде
                
        print(f"[Успешно] Создан: {rel_path}")

    print(f"\n[Готово] Структура '{PROJECT_NAME}' полностью развернута!")

if __name__ == "__main__":
    main()
