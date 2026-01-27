# Architecture Overview

The application is structured as a modular C++ system with clear separation
between data collection, modeling, exporting, and logging.

## Core Principles
- Collectors do not format output
- Exporters do not collect data
- Models are passive data containers
- Errors are logged, not hidden
