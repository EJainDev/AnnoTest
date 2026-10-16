# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "AnnoTest"
copyright = "2026, Ekansh Jain"
author = "Ekansh Jain"
version = "0.2"
release = "0.2.0"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ["sphinx.ext.autodoc", "sphinx.ext.viewcode"]

exclude_patterns = []


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "furo"
html_static_path = ["_static"]
html_css_files = ["home.css"]
templates_path = ["_templates"]

html_theme_options = {
    "navigation_with_keys": True,
}

html_sidebars = {
    "**": [
        "sidebar/brand.html",
        "sidebar/search.html",
        "sidebar/navigation.html",
        "sidebar/ethical-ads.html",
    ]
}


def setup(app):
    # Pygments' attribute state does not yet accept C++26 expression annotations.
    import importlib.util
    from pathlib import Path
    spec = importlib.util.spec_from_file_location('annotest_lexer', Path(__file__).with_name('annotest_lexer.py'))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    app.add_lexer('cpp', module.AnnoTestCppLexer)
    return {'parallel_read_safe': True, 'parallel_write_safe': True}
