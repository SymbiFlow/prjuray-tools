import setuptools

setuptools.setup(
    name="prjuray",
    version="0.0.1",
    author="SymbiFlow Authors",
    author_email="symbiflow@lists.librecores.org",
    description="Project U-Ray database access code",
    long_description="",
    url="https://github.com/SymbiFlow/prjuray-tools",
    packages=setuptools.find_packages(),
    install_requires=['intervaltree', 'simplejson'],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: ISC License",
        "Operating System :: OS Independent",
    ],
)
